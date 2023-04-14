//same logic as grpc/inproc_transport.cc
#include "transport/transport.hpp"
__attribute__((unused)) const static char *TAG = "transport";
namespace erpc {
    bool cancel_stream_locked(inproc_stream *s, grpc_error_handle error);
    void maybe_process_ops_locked(inproc_stream *s, grpc_error_handle error);
    void op_state_machine_locked(inproc_stream *s, grpc_error_handle error);
    void log_metadata(const grpc_metadata_batch *md_batch, bool is_client,
        bool is_initial);
    void fill_in_metadata(inproc_stream *s, const grpc_metadata_batch *metadata,
        grpc_metadata_batch *out_md, bool *markfilled);

    void write_metadata(inproc_stream *s, const grpc_metadata_batch *metadata) {
        s->m_worker->write_metadata(s, metadata);
    }

    void ResetSendMessage(grpc_transport_stream_op_batch *batch) {
        std::exchange(batch->payload->send_message.send_message, nullptr)->Clear();
    }
    
    inproc_stream::inproc_stream(inproc_transport *t,
            const void *server_data)
            : t(t) {
            stream_list_prev = nullptr;
            // gpr_mu_lock(&t->mu->mu);
            stream_list_next = t->stream_list;
            if (t->stream_list) {
                t->stream_list->stream_list_prev = this;
            }
            t->stream_list = this;
            this->listed = true;
            // gpr_mu_unlock(&t->mu->mu);
            if (!server_data) {
                /*DO 尝试连接服务器*/
                rpc_status status = Success;
                struct addrinfo hints = {};
                char portString[8];
                struct addrinfo *res0;
                int result, set;
                int sock = -1;
                struct addrinfo *res;

                if (m_sockfd != -1) {
                    LOGE("%s", "socket already connected, error: %s", strerror(errno));
                }
                else {
                    // Fill in hints structure for getaddrinfo.
                    hints.ai_flags = AI_NUMERICSERV;
                    hints.ai_family = AF_INET;
                    hints.ai_socktype = SOCK_STREAM;

                    // Convert port number to a string.
                    result = snprintf(portString, sizeof(portString), "%d", m_port);
                    if (result < 0) {
                        LOGE(TAG, "snprintf failed, error: %s", strerror(errno));
                        status = Fail;
                    }

                    if (status == Success) {
                        // Perform the name lookup.
                        result = getaddrinfo(m_host, portString, &hints, &res0);
                        if (result != 0) {
                            // TODO check EAI_NONAME
                            LOGE(TAG, "gettaddrinfo failed, error: %s", strerror(errno));
                            status = kErpcStatus_UnknownName;
                        }
                    }

                    if (status == Success) {
                        // Iterate over result addresses and try to connect. Exit the loop on the first successful
                        // connection.
                        for (res = res0; res; res = res->ai_next) {
                            // Create the socket.
                            sock = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
                            if (sock < 0) {
                                LOGW(TAG, "try create a socket, but failed");
                                continue;
                            }
                            // Attempt to connect.
                            char netinfo[24];
                            sprint_net_info(netinfo, sizeof(netinfo), res->ai_addr, res->ai_addrlen);
                            LOGI(TAG, "try to connect to %s", netinfo);
                            if (connect(sock, res->ai_addr, res->ai_addrlen) < 0) {
                                LOGW(TAG, "try to connect for one of socket, but failed");
                                ::close(sock);
                                sock = -1;
                                continue;
                            }
                            else {
                                // Exit the loop for the first successful connection.
                                char netinfo[24];
                                sprint_net_info(netinfo, sizeof(netinfo), res->ai_addr, res->ai_addrlen);
                                LOGI(TAG, "successful connection to %s", netinfo);
                                break;
                            }
                        }

                        // Free the result list.
                        freeaddrinfo(res0);

                        // Check if we were able to open a connection.
                        if (sock < 0) {
                            // TODO check EADDRNOTAVAIL:
                            LOGE(TAG, "connecting failed, error: %s", strerror(errno));
                            status = kErpcStatus_ConnectionFailure;
                        }
                    }

                    if (status == Success) {
                        set = 1;
                        if (setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, (void *) &set, sizeof(int)) < 0) {
                            ::close(sock);
                            LOGE(TAG, "setsockopt failed, error: %s", strerror(errno));
                            status = Fail;
                        }
                    }

                    if (status == Success) {
                        // globally disable the SIGPIPE signal
                        // signal(SIGPIPE, SIG_IGN);
                        m_sockfd = sock;
                    }
                }
                if (Success == status) {
                    /* TODO,准备Worker和Codec */
                    // m_transport_worker = new TCPWorker(m_sockfd, m_port);
                    // m_transport = new NanopbTransport(m_transport_worker, m_messageFactory);
                }
                else {
                    LOGE(TAG, "connecting failed, error: %s", strerror(errno));
                }
            }
            else {
                /*TODO 从serverdata中取出用于建立连接的一些消息.如果是多线程是不是应该建立一个新线程负责这种事情*/
                // This is the server-side and is being called through accept_stream_cb                
                // inproc_stream *cs = const_cast<inproc_stream *>(
                //     static_cast<const inproc_stream *>(server_data));
                // other_side = cs;
                Server *server = static_cast<Server *>(const_cast<void *>(server_data));
                server->m_port = m_port;
                server->m_host = m_host;
                server->m_sockfd = m_sockfd;
                int yes = 1;
                int result;
                struct sockaddr incomingAddress;
                socklen_t incomingAddressLength;
                int incomingSocket;
                bool status = false;
                struct sockaddr_in serverAddress;
                // int epfd;
                // static struct epoll_event events[EPOLL_SIZE];

                LOGI(TAG, "%s", "networkpollerThread");

                // Create socket.
                m_sockfd = socket(AF_INET, SOCK_STREAM, 0);
                if (m_sockfd < 0) {
                    LOGE(TAG, "failed to create server socket, error: %s", strerror(errno));
                    status = true;
                }
                if (!status) {
                    // Fill in address struct.
                    (void) memset(&serverAddress, 0, sizeof(serverAddress));
                    serverAddress.sin_family = AF_INET;
                    serverAddress.sin_addr.s_addr = INADDR_ANY; // htonl(local ? INADDR_LOOPBACK : INADDR_ANY);
                    serverAddress.sin_port = htons(m_port);

                    // Turn on reuse address option.
                    result = setsockopt(m_sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));
                    if (result < 0) {
                        LOGE(TAG, "setsockopt failed, error: %s", strerror(errno));
                        status = true;
                    }
                }

                if (!status) {
                    // Bind socket to address.
                    result = bind(m_sockfd, (struct sockaddr *) &serverAddress, sizeof(serverAddress));
                    //on Failed
                    if (result < 0) {
                        LOGE(TAG, "bind failed, error: %s", strerror(errno));
                        status = true;
                    }
                    else {
                        //on Success
                        char netinfo[24];
                        sprint_net_info(netinfo, sizeof(netinfo), (struct sockaddr *) &serverAddress, sizeof(serverAddress));
                        LOGI(TAG, "bind to %s", netinfo);
                    }

                }

                if (!status) {
                    // Listen for connections.
                    result = listen(m_sockfd, 4);
                    //on Failed
                    if (result < 0) {
                        LOGE(TAG, "listen failed, error: %s", strerror(errno));
                        status = true;
                    }
                    //on Success
                    LOGI(TAG, "%s", "Listening for connections\n");
                }
                
                /*跳过一些连接过程,但是我们显然不能这么做*/
                // Now we are about to affect the other side, so lock the transport
                // to make sure that it doesn't get destroyed
                // gpr_mu_lock(&t->mu->mu);
                // cs->other_side = this;
                // Now transfer from the other side's write_buffer if any to the to_read
                // buffer
                // if (cs->write_buffer_initial_md_filled) {
                //     fill_in_metadata(this, &cs->write_buffer_initial_md,
                //         &to_read_initial_md, &to_read_initial_md_filled);
                //     // deadline = std::min(deadline, cs->write_buffer_deadline);
                //     cs->write_buffer_initial_md.Clear();
                //     cs->write_buffer_initial_md_filled = false;
                // }
                // // if (cs->write_buffer_trailing_md_filled) {
                //     fill_in_metadata(this, &cs->write_buffer_trailing_md,
                //         &to_read_trailing_md, &to_read_trailing_md_filled);
                //     cs->write_buffer_trailing_md.Clear();
                //     cs->write_buffer_trailing_md_filled = false;
                // }
                /*如果检测到对方错误就关闭自己,我们也不能这么做*/
                // if (rpc_status::Success != cs->write_buffer_cancel_error) {
                //     cancel_other_error = cs->write_buffer_cancel_error;
                //     cs->write_buffer_cancel_error = rpc_status::Success;
                //     maybe_process_ops_locked(this, cancel_other_error);
                // }

                // gpr_mu_unlock(&t->mu->mu);
            }
        }

    // class inproc_transport {
    // public:
    //     inproc_transport(bool is_client)
    //         : //mu(mu),
    //         is_client(is_client)
    //         // ,state_tracker(is_client ? "inproc_client" : "inproc_server",
    //             // GRPC_CHANNEL_READY) {
    //     {
    //         // Start each side of transport with 2 refs since they each have a ref
    //         // to the other
    //         // gpr_ref_init(&refs, 2);
    //     }
    //     ~inproc_transport() {
    //         //            mu->~shared_mu();
    //         //            gpr_free(mu);
    //     }
    //     // shared_mu* mu;
    //     bool is_client;
    //     //    ConnectivityStateTracker state_tracker;
    //     void (*accept_stream_cb)(void *user_data, const void *server_data);
    //     void *accept_stream_data;
    //     bool is_closed = false;
    //     Server *watcher = nullptr;
    //     inproc_transport *other_side;
    //     inproc_stream *stream_list = nullptr;
    // };

    void close_stream_locked(inproc_stream *s) {
        if (!s->closed) {
            // Release the metadata that we would have written out
            s->write_buffer_initial_md.Clear();
            s->write_buffer_trailing_md.Clear();
            
            // Remove from the transport's list of streams
            if (s->listed) {
                inproc_stream *p = s->stream_list_prev;
                inproc_stream *n = s->stream_list_next;
                if (p != nullptr) {
                    p->stream_list_next = n;
                }
                else {
                    s->t->stream_list = n;
                }
                if (n != nullptr) {
                    n->stream_list_prev = p;
                }
                s->listed = false;
                // s->unref("close_stream:list");
            }
            s->closed = true;
            //DO 关掉自己
            close(s->m_sockfd);
            // s->unref("close_stream:closing");
        }
    }

    /* 我们没法关掉对方,但是可以把自己关掉 */
    // This function means that we are done talking/listening to the other side
    void close_other_side_locked(inproc_stream *s, const char *reason) {
        LOGE(TAG, "close_other_side_locked %p %s", s, reason);
        //DO 关掉自己
        close(s->m_sockfd);
    }

// Call the on_complete closure associated with this stream_op_batch if
// this stream_op_batch is only one of the pending operations for this
// stream. This is called when one of the pending operations for the stream
// is done and about to be NULLed out
    /* 状态机-barrier 调度, 维持不变 */
    void complete_if_batch_end_locked(inproc_stream *s, rpc_status error,
        grpc_transport_stream_op_batch *op,
        const char *msg) {
        int is_sm = static_cast<int>(op == s->send_message_op);
        int is_stm = static_cast<int>(op == s->send_trailing_md_op);
        // TODO(vjpai): We should not consider the recv ops here, since they
        // have their own callbacks.  We should invoke a batch's on_complete
        // as soon as all of the batch's send ops are complete, even if there
        // are still recv ops pending.
        int is_rim = static_cast<int>(op == s->recv_initial_md_op);
        int is_rm = static_cast<int>(op == s->recv_message_op);
        int is_rtm = static_cast<int>(op == s->recv_trailing_md_op);

        if ((is_sm + is_stm + is_rim + is_rm + is_rtm) == 1) {
            LOGI(TAG, "%s %p %p %d", msg, s, op, error);
            op->on_complete->run(error);
        }
    }

    /* 状态机-需要读取或者出现错误, 维持不变 */
    void maybe_process_ops_locked(inproc_stream *s, grpc_error_handle error) {
        if (s && (rpc_status::Success != error || s->ops_needed)) {
            s->ops_needed = false;
            op_state_machine_locked(s, error);
        }
    }

    /* 需要改变,不能这么简单的发送数据 */
    void message_transfer_locked(inproc_stream *sender) {

        /*DO 我们只能发自己的数据并且让别人接收数据 */
        /* 我们将消息和对应的flag进行对应设置并且发送*/
        std::string to_send = sender->send_message_op->payload->send_message.send_message->JoinIntoString();
        sender->m_worker->send(reinterpret_cast<const uint8_t*>(to_send.c_str()), to_send.size());
        // sender->send_message_op->payload->send_message.flags;

        complete_if_batch_end_locked(
            sender, rpc_status::Success, sender->send_message_op,
         "message_transfer scheduling sender on_complete");
        sender->send_message_op = nullptr;
        
        // *receiver->recv_message_op->payload->recv_message.recv_message =
        //     std::move(*sender->send_message_op->payload->send_message.send_message);
        // *receiver->recv_message_op->payload->recv_message.flags =
        //     sender->send_message_op->payload->send_message.flags;

        // LOGI(TAG, "message_transfer_locked %p scheduling message-ready",
        //     receiver);

        // receiver->recv_message_op->payload->recv_message.recv_message_ready->run(rpc_status::Success);
        // complete_if_batch_end_locked(
        //     sender, rpc_status::Success, sender->send_message_op,
        //     "message_transfer scheduling sender on_complete");
        // complete_if_batch_end_locked(
        //     receiver, rpc_status::Success, receiver->recv_message_op,
        //     "message_transfer scheduling receiver on_complete");
        // receiver->recv_message_op = nullptr;
        // sender->send_message_op = nullptr;
    }

    /* 状态机-失败后的优雅推出, 需要改变一些填充信息相关内容*/
    void fail_helper_locked(inproc_stream *s, grpc_error_handle error) {
        LOGI(TAG, "op_state_machine %p fail_helper", s);
        // If we're failing this side, we need to make sure that
        // we also send or have already sent trailing metadata
        if (!s->trailing_md_sent) {
            //如果还没有发送元数据，那么就发送元数据表示取消调用
            s->trailing_md_sent = true;

            grpc_metadata_batch fake_md{};

            //TODO 发送远程信息，正确设置write_buffer_cancel_error
            // fill_in_metadata(s, fake_md, dest, destfilled);

            // grpc_metadata_batch *dest = (other == nullptr)
            //     ? &s->write_buffer_trailing_md
            //     : &other->to_read_trailing_md;
            // bool *destfilled = (other == nullptr) ? &s->write_buffer_trailing_md_filled
            //     : &other->to_read_trailing_md_filled;
            // fill_in_metadata(s, &fake_md, dest, destfilled);

            // if (other != nullptr) {
            //     if (other->cancel_other_error.ok()) {
            //         other->cancel_other_error = error;
            //     }
            //     maybe_process_ops_locked(other, error);
            // }
            // else if (s->write_buffer_cancel_error.ok()) {
            //     s->write_buffer_cancel_error = error;
            // }
        }
        if (s->recv_initial_md_op) {
            grpc_error_handle err;
            //失败后的处理：填入虚假元数据
            if (!s->t->is_client) {
                // If this is a server, provide initial metadata with a path and
                // authority since it expects that as well as no error yet
                grpc_metadata_batch *fake_md = new grpc_metadata_batch();
                // 为fake设置合适的字段
                
                // fake_md.Set(grpc_core::HttpPathMetadata(),
                    // grpc_core::Slice::FromStaticString("/"));
                // fake_md.Set(grpc_core::HttpAuthorityMetadata(),
                    // grpc_core::Slice::FromStaticString("inproc-fail"));

                //TODO 正确给自己填写虚假的元数据
                fill_in_metadata(s, fake_md,
                    s->recv_initial_md_op->payload->recv_initial_metadata
                    .recv_initial_metadata,
                    nullptr);
                err = rpc_status::Success;
            }//如果是客户端，则不应该有这种事情发生，直接报错
            else {
                err = error;
            }
            //失败后的处理：设置元数据已经接收成功
            if (s->recv_initial_md_op->payload->recv_initial_metadata
                .trailing_metadata_available != nullptr) {
                // Set to true unconditionally, because we're failing the call, so even
                // if we haven't actually seen the send_trailing_metadata op from the
                // other side, we're going to return trailing metadata anyway.
                *s->recv_initial_md_op->payload->recv_initial_metadata
                    .trailing_metadata_available = true;
            }
            LOGI(TAG,
                "fail_helper %p scheduling initial-metadata-ready %s %s", s,
                StatusToString(error),
                StatusToString(err));
            
            //失败后的处理：调用回调
            s->recv_initial_md_op->payload->recv_initial_metadata
                .recv_initial_metadata_ready->run(err);
            // Last use of err so no need to REF and then UNREF it
            complete_if_batch_end_locked(
                s, error, s->recv_initial_md_op,
                "fail_helper scheduling recv-initial-metadata-on-complete");
            s->recv_initial_md_op = nullptr;
        }
        if (s->recv_message_op) {
            // LOGI(TAG,  "fail_helper %p scheduling message-ready %s", s,
                // StatusToString(error));
            //直接从现有状态继续
            if (s->recv_message_op->payload->recv_message
                .call_failed_before_recv_message != nullptr) {
                *s->recv_message_op->payload->recv_message
                    .call_failed_before_recv_message = true;
            }
            s->recv_message_op->payload->recv_message.recv_message_ready->run(error);
            complete_if_batch_end_locked(
                s, error, s->recv_message_op,
                "fail_helper scheduling recv-message-on-complete");
            s->recv_message_op = nullptr;
        }
        if (s->send_message_op) {
            //TODO 正确重置缓冲区，取消发送。
            ResetSendMessage(s->send_message_op);
            complete_if_batch_end_locked(
                s, error, s->send_message_op,
                "fail_helper scheduling send-message-on-complete");
            s->send_message_op = nullptr;
        }
        if (s->send_trailing_md_op) {
            //完成发送
            complete_if_batch_end_locked(
                s, error, s->send_trailing_md_op,
                "fail_helper scheduling send-trailng-md-on-complete");
            s->send_trailing_md_op = nullptr;
        }
        if (s->recv_trailing_md_op) {
            /* 直接认为接收成功 */
            LOGI(TAG, "fail_helper %p scheduling trailing-metadata-ready %s",
                s, StatusToString(error));
            s->recv_trailing_md_op->payload->recv_trailing_metadata
                .recv_trailing_metadata_ready->run(error);
            LOGI(TAG, "fail_helper %p scheduling trailing-md-on-complete %s",
                s, StatusToString(error));
            complete_if_batch_end_locked(
                s, error, s->recv_trailing_md_op,
                "fail_helper scheduling recv-trailing-metadata-on-complete");
            s->recv_trailing_md_op = nullptr;
        }
        close_other_side_locked(s, "fail_helper:other_side");
        close_stream_locked(s);
    }
    
    /* 状态机-读取一些内容,我们把这个东西改成同步的更有益于身心健康....*/
    void op_state_machine_locked(inproc_stream *s, grpc_error_handle error) {
        // This function gets called when we have contents in the unprocessed reads
        // Get what we want based on our ops wanted
        // Schedule our appropriate closures
        // and then return to ops_needed state if still needed

        grpc_error_handle new_err;

        bool needs_close = false;

        LOGI(TAG, "op_state_machine %p", s);
        // cancellation takes precedence
        // inproc_stream *other = s->other_side;

        if (rpc_status::Success != s->cancel_self_error) {
            fail_helper_locked(s, s->cancel_self_error);
            goto done;
        }
        // else if (rpc_status::Success != s->cancel_other_error) {
        //     fail_helper_locked(s, s->cancel_other_error);
        //     goto done;
        // }
        else if (rpc_status::Success != error) {
            fail_helper_locked(s, error);
            goto done;
        }

        if (s->send_message_op) {
            /* TODO 正确发送消息 */
            //message_transfer_locked(s, other);
            /* TODO 正确触发该回调 (从recv)*/
           // maybe_process_ops_locked(other, rpc_status::Success);

            if (!s->t->is_client && s->trailing_md_sent) {
                // A server send will never be matched if the server already sent status
                ResetSendMessage(s->send_message_op);
                complete_if_batch_end_locked(
                    s, rpc_status::Success, s->send_message_op,
                    "op_state_machine scheduling send-message-on-complete case 1");
                s->send_message_op = nullptr;
            }
        }
        // Pause a send trailing metadata if there is still an outstanding
        // send message unless we know that the send message will never get
        // matched to a receive. This happens on the client if the server has
        // already sent status or on the server if the client has requested
        // status
        if (s->send_trailing_md_op &&
            (!s->send_message_op ||
                (s->t->is_client &&
                    (s->trailing_md_recvd || s->to_read_trailing_md_filled))
                /* TODO 给服务端编写何时应该发送md的逻辑 */
                //  || (!s->t->is_client && other &&
                //     (other->trailing_md_recvd || other->to_read_trailing_md_filled ||
                //         other->recv_trailing_md_op))
                )) {
            /* TODO 可能不需要干啥 */
            //     grpc_metadata_batch *dest = (other == nullptr)
            //     ? &s->write_buffer_trailing_md
            //     : &other->to_read_trailing_md;
            // bool *destfilled = (other == nullptr) ? &s->write_buffer_trailing_md_filled
            //     : &other->to_read_trailing_md_filled;
            if (s->trailing_md_sent) {
                // The buffer is already in use; that's an error!
                LOGI(TAG, "Extra trailing metadata %p", s);
                new_err = rpc_status::UnexpectedTransportOperation;
                fail_helper_locked(s, new_err);
                goto done;
            }
            else {
                /* TODO 正确发送 */
                // fill_in_metadata(s,
                //         s->send_trailing_md_op->payload->send_trailing_metadata
                //         .send_trailing_metadata,
                //         dest, destfilled);
                s->trailing_md_sent = true;
                if (s->send_trailing_md_op->payload->send_trailing_metadata.sent) {
                    *s->send_trailing_md_op->payload->send_trailing_metadata.sent = true;
                }
                /* 对于服务端,这将完成发送 */
                if (!s->t->is_client && s->trailing_md_recvd && s->recv_trailing_md_op) {
                    LOGI(TAG,
                        "op_state_machine %p scheduling trailing-metadata-ready", s);

                    s->recv_trailing_md_op->payload->recv_trailing_metadata
                        .recv_trailing_metadata_ready->run(rpc_status::Success);
                    LOGI(TAG,
                        "op_state_machine %p scheduling trailing-md-on-complete", s);

                    s->recv_trailing_md_op->on_complete->run(rpc_status::Success);
                    s->recv_trailing_md_op = nullptr;
                    needs_close = true;
                }
            }
            /* 正确调用这个回调,让对方去读取 */
            // maybe_process_ops_locked(other, rpc_status::Success);
            complete_if_batch_end_locked(
                s, rpc_status::Success, s->send_trailing_md_op,
                "op_state_machine scheduling send-trailing-metadata-on-complete");
            s->send_trailing_md_op = nullptr;
        }
        if (s->recv_initial_md_op) {
            if (s->initial_md_recvd) {
                new_err = rpc_status::UnexpectedTransportOperation;
                LOGE(TAG, "Already recvd initial md");
                LOGI(TAG,
                    "op_state_machine %p scheduling on_complete errors for already "
                    "recvd initial md %s",
                    s, StatusToString(new_err));
                fail_helper_locked(s, new_err);
                goto done;
            }
            /* TODO 是否应该发起一个阻塞读? */
            if (s->to_read_initial_md_filled) {
                s->initial_md_recvd = true;
                /* 正确使用这一函数 */
                fill_in_metadata(s, &s->to_read_initial_md,
                    s->recv_initial_md_op->payload->recv_initial_metadata
                    .recv_initial_metadata,
                    nullptr);
                if (s->deadline != Timestamp::InfFuture()) {
                    s->recv_initial_md_op->payload->recv_initial_metadata
                        .recv_initial_metadata->SetTimeout(s->deadline);
                }
                if (s->recv_initial_md_op->payload->recv_initial_metadata
                    .trailing_metadata_available != nullptr) {
                    *s->recv_initial_md_op->payload->recv_initial_metadata
                        .trailing_metadata_available =
                        false;
                        /* TODO 或许有办法设置这个东西,但应该不是现在 */
                        // (other != nullptr && other->send_trailing_md_op != nullptr);
                }
                s->to_read_initial_md.Clear();
                s->to_read_initial_md_filled = false;

                s->recv_initial_md_op->payload->recv_initial_metadata
                    .recv_initial_metadata_ready->run(
                        rpc_status::Success);
                complete_if_batch_end_locked(
                    s, rpc_status::Success, s->recv_initial_md_op,
                    "op_state_machine scheduling recv-initial-metadata-on-complete");
                s->recv_initial_md_op = nullptr;
            }
        }
        if (s->recv_message_op) {
            /* 应该进行一个阻塞读,然后正确调用 */
            // if (other && other->send_message_op) {
            //     message_transfer_locked(other, s);
            //     maybe_process_ops_locked(other, rpc_status::Success);
            // }
        }
        if (s->to_read_trailing_md_filled) {
            if (s->trailing_md_recvd) {
                if (s->trailing_md_recvd_implicit_only) {
                    LOGI(TAG,
                        "op_state_machine %p already implicitly received trailing "
                        "metadata, so ignoring new trailing metadata from client",
                        s);
                    s->to_read_trailing_md.Clear();
                    s->to_read_trailing_md_filled = false;
                    s->trailing_md_recvd_implicit_only = false;
                }
                else {
                    LOGE(TAG, "Already recvd trailing md");
                    new_err = rpc_status::UnexpectedTransportOperation;
                    LOGI(TAG,
                        "op_state_machine %p scheduling on_complete errors for already "
                        "recvd trailing md %s",
                        s, StatusToString(new_err));
                    fail_helper_locked(s, new_err);
                    goto done;
                }
            }
            /* 不再继续读消息 */
            if (s->recv_message_op != nullptr) {
                // This message needs to be wrapped up because it will never be
                // satisfied
                s->recv_message_op->payload->recv_message.recv_message->~MessageBufferList();
                LOGI(TAG, "op_state_machine %p scheduling message-ready", s);

                s->recv_message_op->payload->recv_message.recv_message_ready->run(
                    rpc_status::Success);
                complete_if_batch_end_locked(
                    s, new_err, s->recv_message_op,
                    "op_state_machine scheduling recv-message-on-complete");
                s->recv_message_op = nullptr;
            }
            /* 不再尝试发消息 */
            if ((s->trailing_md_sent || s->t->is_client) && s->send_message_op) {
                // Nothing further will try to receive from this stream, so finish off
                // any outstanding send_message op
                ResetSendMessage(s->send_message_op);
                s->send_message_op->payload->send_message.stream_write_closed = true;
                complete_if_batch_end_locked(
                    s, new_err, s->send_message_op,
                    "op_state_machine scheduling send-message-on-complete case 2");
                s->send_message_op = nullptr;
            }
            /* 接收尾数据 */
            if (s->recv_trailing_md_op != nullptr) {
                // We wanted trailing metadata and we got it
                s->trailing_md_recvd = true;
                /* TODO 正确进行该传输 */
                fill_in_metadata(s, &s->to_read_trailing_md,
                    s->recv_trailing_md_op->payload->recv_trailing_metadata
                    .recv_trailing_metadata,
                    nullptr);
                s->to_read_trailing_md.Clear();
                s->to_read_trailing_md_filled = false;

                // We should schedule the recv_trailing_md_op completion if
                // 1. this stream is the client-side
                // 2. this stream is the server-side AND has already sent its trailing md
                //    (If the server hasn't already sent its trailing md, it doesn't
                //    have
                //     a final status, so don't mark this op complete)
                if (s->t->is_client || s->trailing_md_sent) {

                    s->recv_trailing_md_op->payload->recv_trailing_metadata
                        .recv_trailing_metadata_ready->run(
                            rpc_status::Success);

                    s->recv_trailing_md_op->on_complete->run(
                        rpc_status::Success);
                    s->recv_trailing_md_op = nullptr;
                    needs_close = s->trailing_md_sent;
                }
            }
            else if (!s->trailing_md_recvd) {
                LOGI(TAG,
                    "op_state_machine %p has trailing md but not yet waiting for it", s);
            }
        }
        if (!s->t->is_client && s->trailing_md_sent &&
            (s->recv_trailing_md_op != nullptr)) {
            // In this case, we don't care to receive the write-close from the client
            // because we have already sent status and the RPC is over as far as we
            // are concerned.
            LOGI(TAG, "op_state_machine %p scheduling trailing-md-ready %s",
                s, StatusToString(new_err));

            s->recv_trailing_md_op->payload->recv_trailing_metadata
                .recv_trailing_metadata_ready->run(
                    new_err);
            complete_if_batch_end_locked(
                s, new_err, s->recv_trailing_md_op,
                "op_state_machine scheduling recv-trailing-md-on-complete");
            s->trailing_md_recvd = true;
            s->recv_trailing_md_op = nullptr;
            // Since we are only pretending to have received the trailing MD, it would
            // be ok (not an error) if the client actually sends it later.
            s->trailing_md_recvd_implicit_only = true;
        }
        if (s->trailing_md_recvd && s->recv_message_op) {
            // No further message will come on this stream, so finish off the
            // recv_message_op
            LOGI(TAG, "op_state_machine %p scheduling message-ready", s);
            s->recv_message_op->payload->recv_message.recv_message->~MessageBufferList();

            s->recv_message_op->payload->recv_message.recv_message_ready->run(
                rpc_status::Success);
            complete_if_batch_end_locked(
                s, new_err, s->recv_message_op,
                "op_state_machine scheduling recv-message-on-complete");
            s->recv_message_op = nullptr;
        }
        if (s->trailing_md_recvd && s->send_message_op && s->t->is_client) {
            // Nothing further will try to receive from this stream, so finish off
            // any outstanding send_message op
            ResetSendMessage(s->send_message_op);
            complete_if_batch_end_locked(
                s, new_err, s->send_message_op,
                "op_state_machine scheduling send-message-on-complete case 3");
            s->send_message_op = nullptr;
        }
        if (s->send_message_op || s->send_trailing_md_op || s->recv_initial_md_op ||
            s->recv_message_op || s->recv_trailing_md_op) {
            // Didn't get the item we wanted so we still need to get
            // rescheduled
            LOGI(TAG, "op_state_machine %p still needs closure %p %p %p %p %p", s,
                s->send_message_op, s->send_trailing_md_op, s->recv_initial_md_op,
                s->recv_message_op, s->recv_trailing_md_op);
            s->ops_needed = true;
        }
    done:
        if (needs_close) {
            close_other_side_locked(s, "op_state_machine");
            close_stream_locked(s);
        }
    }

    bool cancel_stream_locked(inproc_stream *s, grpc_error_handle error) {
        bool ret = false;  // was the cancel accepted
        LOGI(TAG, "cancel_stream %p with %s", s,
            StatusToString(error));
        if (rpc_status::Success != s->cancel_self_error) {
            ret = true;
            s->cancel_self_error = error;
            // Catch current value of other before it gets closed off
            // inproc_stream *other = s->other_side;
            maybe_process_ops_locked(s, s->cancel_self_error);
            // Send trailing md to the other side indicating cancellation, even if we
            // already have
            s->trailing_md_sent = true;

            grpc_metadata_batch cancel_md{};

            // grpc_metadata_batch *dest = (other == nullptr)
            //     ? &s->write_buffer_trailing_md
            //     : &other->to_read_trailing_md;
            // bool *destfilled = (other == nullptr) ? &s->write_buffer_trailing_md_filled
            //     : &other->to_read_trailing_md_filled;
            /* TODO正确发送尾数据 */
            // fill_in_metadata(s, &cancel_md, dest, destfilled);

            // if (other != nullptr) {
            //     if (rpc_status::Success != other->cancel_other_error) {
            //         other->cancel_other_error = s->cancel_self_error;
            //     }
            //     maybe_process_ops_locked(other, other->cancel_other_error);
            // }else 
            if (rpc_status::Success != s->write_buffer_cancel_error) {
                s->write_buffer_cancel_error = s->cancel_self_error;
            }

            // if we are a server and already received trailing md but
            // couldn't complete that because we hadn't yet sent out trailing
            // md, now's the chance
            if (!s->t->is_client && s->trailing_md_recvd && s->recv_trailing_md_op) {

                s->recv_trailing_md_op->payload->recv_trailing_metadata
                    .recv_trailing_metadata_ready->run(
                        s->cancel_self_error);
                complete_if_batch_end_locked(
                    s, s->cancel_self_error, s->recv_trailing_md_op,
                    "cancel_stream scheduling trailing-md-on-complete");
                s->recv_trailing_md_op = nullptr;
            }
        }

        close_other_side_locked(s, "cancel_stream:other_side");
        close_stream_locked(s);

        return ret;
}

    void do_nothing(void * /*arg*/, grpc_error_handle /*error*/) {}
    using grpc_transport = erpc::inproc_transport;
    using grpc_stream = erpc::inproc_stream;

    /* 我们按照doc/core/transport_explainer.md中的提示填写op，至于这个状态机，还是留着他吧或许，但要注意数据相关！ */
    void perform_stream_op(grpc_transport *gt, grpc_stream *gs,
        grpc_transport_stream_op_batch *op) {
        // LOGI(TAG, "perform_stream_op %p %p %p", gt, gs, op);
        inproc_stream *s = reinterpret_cast<inproc_stream *>(gs);
        // gpr_mu *mu = &s->t->mu->mu;  // save aside in case s gets closed
        // gpr_mu_lock(mu);

        // if (GRPC_TRACE_FLAG_ENABLED(grpc_inproc_trace)) {
        //     if (op->send_initial_metadata) {
        //         log_metadata(op->payload->send_initial_metadata.send_initial_metadata,
        //             s->t->is_client, true);
        //     }
        //     if (op->send_trailing_metadata) {
        //         log_metadata(op->payload->send_trailing_metadata.send_trailing_metadata,
        //             s->t->is_client, false);
        //     }
        // }
        grpc_error_handle error;
        grpc_closure *on_complete = op->on_complete;
        // TODO(roth): This is a hack needed because we use data inside of the
        // closure itself to do the barrier calculation (i.e., to ensure that
        // we don't schedule the closure until all ops in the batch have been
        // completed).  This can go away once we move to a new C++ closure API
        // that provides the ability to create a barrier closure.
        if (on_complete == nullptr) {
            on_complete = grpc_closure_init(&op->handler_private.closure, do_nothing, nullptr);
        }

        if (op->cancel_stream) {
            // Call cancel_stream_locked without ref'ing the cancel_error because
            // this function is responsible to make sure that that field gets unref'ed
            cancel_stream_locked(s, op->payload->cancel_stream.cancel_error);
            // this op can complete without an error
        }
        else if (rpc_status::Success != !s->cancel_self_error) {
            // already self-canceled so still give it an error
            error = s->cancel_self_error;
        }
        else {
            LOGI(TAG, "perform_stream_op %p %s%s%s%s%s%s%s", s,
                s->t->is_client ? "client" : "server",
                op->send_initial_metadata ? " send_initial_metadata" : "",
                op->send_message ? " send_message" : "",
                op->send_trailing_metadata ? " send_trailing_metadata" : "",
                op->recv_initial_metadata ? " recv_initial_metadata" : "",
                op->recv_message ? " recv_message" : "",
                op->recv_trailing_metadata ? " recv_trailing_metadata" : "");
        }

        // inproc_stream *other = s->other_side;
        if (rpc_status::Success != error && (op->send_initial_metadata || op->send_trailing_metadata)) {
            if (s->t->is_closed) {
                LOGI(TAG, "Endpoint already shutdown");
                error = rpc_status::UnexpectedTransportOperation;
            }
            if (rpc_status::Success != error && op->send_initial_metadata) {
                // grpc_metadata_batch *dest = (other == nullptr)
                //     ? &s->write_buffer_initial_md
                //     : &other->to_read_initial_md;
                // bool *destfilled = (other == nullptr) ? &s->write_buffer_initial_md_filled
                //     : &other->to_read_initial_md_filled;
                if (s->initial_md_sent) {
                    // The buffer is already in use; that's an error!
                    LOGI(TAG, "Extra initial metadata %p", s);
                    error = rpc_status::UnexpectedTransportOperation;
                }
                else {
                    if (!s->other_side_closed) {
                        /* TODO 正确传送初始元数据需要更换 */
                        // fill_in_metadata(
                        //     s, op->payload->send_initial_metadata.send_initial_metadata, dest,
                        //     destfilled);
                    }
                    if (s->t->is_client) {
                        Timestamp *dl =  &s->write_buffer_deadline;
                        *dl = std::min(
                            *dl, op->payload->send_initial_metadata.send_initial_metadata
                            ->get_timeout());
                        s->initial_md_sent = true;
                    }
                }
                /* TODO 对方应该在非阻塞进程中自己调用这个东西 */
                // maybe_process_ops_locked(other, error);
            }
        }

        if (rpc_status::Success != error && (op->send_message || op->send_trailing_metadata ||
            op->recv_initial_metadata || op->recv_message ||
            op->recv_trailing_metadata)) {
            // Mark ops that need to be processed by the state machine
            if (op->send_message) {
                s->send_message_op = op;
            }
            if (op->send_trailing_metadata) {
                s->send_trailing_md_op = op;
            }
            if (op->recv_initial_metadata) {
                s->recv_initial_md_op = op;
            }
            if (op->recv_message) {
                s->recv_message_op = op;
            }
            if (op->recv_trailing_metadata) {
                s->recv_trailing_md_op = op;
            }

            // We want to initiate the state machine if:
            // 1. We want to send a message and the other side wants to receive
            // 2. We want to send trailing metadata and there isn't an unmatched send
            //    or the other side wants trailing metadata
            // 3. We want initial metadata and the other side has sent it
            // 4. We want to receive a message and there is a message ready
            // 5. There is trailing metadata, even if nothing specifically wants
            //    that because that can shut down the receive message as well
            // if ((op->send_message && other && other->recv_message_op != nullptr) ||
            //     (op->send_trailing_metadata &&
            //         (!s->send_message_op || (other && other->recv_trailing_md_op))) ||
            //     (op->recv_initial_metadata && s->to_read_initial_md_filled) ||
            //     (op->recv_message && other && other->send_message_op != nullptr) ||
            //     (s->to_read_trailing_md_filled || s->trailing_md_recvd))
            if ((op->send_message) ||
                    (op->send_trailing_metadata && !s->send_message_op) ||
                    (op->recv_initial_metadata && s->to_read_initial_md_filled) ||
                    (op->recv_message) ||
                (s->to_read_trailing_md_filled || s->trailing_md_recvd)) {
                op_state_machine_locked(s, error);
            }

            else {
                s->ops_needed = true;
            }
        }
        else {
            if (rpc_status::Success != error) {
                // Consume any send message that was sent here but that we are not
                // pushing to the other side
                if (op->send_message) {
                    ResetSendMessage(op);
                }
                // Schedule op's closures that we didn't push to op state machine
                if (op->recv_initial_metadata) {
                    if (op->payload->recv_initial_metadata.trailing_metadata_available !=
                        nullptr) {
                        // Set to true unconditionally, because we're failing the call, so
                        // even if we haven't actually seen the send_trailing_metadata op
                        // from the other side, we're going to return trailing metadata
                        // anyway.
                        *op->payload->recv_initial_metadata.trailing_metadata_available =
                            true;
                    }
                    LOGI(TAG,
                        "perform_stream_op error %p scheduling initial-metadata-ready %s",
                        s, StatusToString(error));

                    op->payload->recv_initial_metadata.recv_initial_metadata_ready->run(
                        error);
                }
                if (op->recv_message) {
                    LOGI(TAG,
                        "perform_stream_op error %p scheduling recv message-ready %s", s,
                        StatusToString(error));
                    if (op->payload->recv_message.call_failed_before_recv_message !=
                        nullptr) {
                        *op->payload->recv_message.call_failed_before_recv_message = true;
                    }

                    op->payload->recv_message.recv_message_ready->run(
                        error);
                }
                if (op->recv_trailing_metadata) {
                    LOGI(TAG,
                        "perform_stream_op error %p scheduling "
                        "trailing-metadata-ready %s",
                        s, StatusToString(error));

                    op->payload->recv_trailing_metadata.recv_trailing_metadata_ready->run(
                        error);
                }
            }
            LOGI(TAG, "perform_stream_op %p scheduling on_complete %s", s,
                StatusToString(error));
            on_complete->run(error);
        }
        // gpr_mu_unlock(mu);
    }

    void close_transport_locked(inproc_transport *t) {
        LOGI(TAG, "close_transport %p %d", t, t->is_closed);
        // t->state_tracker.SetState(GRPC_CHANNEL_SHUTDOWN, absl::Status(),
            // "close transport");
        if (!t->is_closed) {
            t->is_closed = true;
            /* Also end all streams on this transport */
            while (t->stream_list != nullptr) {
                // cancel_stream_locked also adjusts stream list
                cancel_stream_locked(
                    t->stream_list,
                    rpc_status::kErpcStatus_ConnectionClosed);
            }
        }
    }
    
    /* 我们使用同步RPC不用手动调度填写回调，直接将这个op变成一个server里的流程就可以了 */
    void perform_transport_op(grpc_transport *gt, grpc_transport_op *op) {
        inproc_transport *t = reinterpret_cast<inproc_transport *>(gt);
        LOGI(TAG, "perform_transport_op %p %p", t, op);
        // gpr_mu_lock(&t->mu->mu);
        if (op->start_connectivity_watch != nullptr) {
            /* 将服务器加入监听列表 */
            LOGI(TAG, "perform_transport_op %p start_connectivity_watch", t);
            t->watcher = op->start_connectivity_watch;
        }
        // if (op->stop_connectivity_watch != nullptr) {
        //     LOGI(TAG, "perform_transport_op %p stop_connectivity_watch", t);
        //     // t->state_tracker.RemoveWatcher(op->stop_connectivity_watch);
        // }
        if (op->set_accept_stream) {
            /*我们在服务器开始之后就在主线程上手动调用它,将callback设置为*/
            LOGI(TAG, "perform_transport_op %p accept_stream", t);
            t->accept_stream_cb = op->set_accept_stream_fn;
            t->accept_stream_data = op->set_accept_stream_user_data;
        }
        if (op->on_consumed) {
            /* 暂时不知道可以拿来做什么（？或许可以用来进行底层调用 */
            LOGI(TAG, "perform_transport_op %p on_consumed", t);
            op->on_consumed->run(rpc_status::Success);
        }

        bool do_close = false;
        if (rpc_status::Success != !op->goaway_error) {
            do_close = true;
        }
        if (rpc_status::Success != !op->disconnect_with_error) {
            do_close = true;
        }

        if (do_close) {
            /* 什么情况下会想要关闭传输层呢？对应于出错的情况。 */
            LOGI(TAG, "perform_transport_op %p do_close", t);
            close_transport_locked(t);
        }
        // gpr_mu_unlock(&t->mu->mu);
    }

    void destroy_stream(grpc_transport *gt, grpc_stream *gs,
        grpc_closure *then_schedule_closure) {
        LOGI(TAG, "destroy_stream %p %p", gs, then_schedule_closure);
        inproc_transport *t = reinterpret_cast<inproc_transport *>(gt);
        inproc_stream *s = reinterpret_cast<inproc_stream *>(gs);
        // gpr_mu_lock(&t->mu->mu);
        close_stream_locked(s);
        // gpr_mu_unlock(&t->mu->mu);
        s->~inproc_stream();
        then_schedule_closure->run(
            rpc_status::Success);
    }

    void destroy_transport(grpc_transport *gt) {
        inproc_transport *t = reinterpret_cast<inproc_transport *>(gt);
        LOGI(TAG, "destroy_transport %p", t);
        // gpr_mu_lock(&t->mu->mu);
        close_transport_locked(t);
        // gpr_mu_unlock(&t->mu->mu);
        // t->other_side->unref();
        // t->unref();
    }

/*******************************************************************************
 * INTEGRATION GLUE
 */

// void set_pollset(grpc_transport * /*gt*/, grpc_stream * /*gs*/,
//     grpc_pollset * /*pollset*/) {
//     // Nothing to do here
// }

// void set_pollset_set(grpc_transport * /*gt*/, grpc_stream * /*gs*/,
//     grpc_pollset_set * /*pollset_set*/) {
//     // Nothing to do here
// }

// grpc_endpoint *get_endpoint(grpc_transport * /*t*/) { return nullptr; }


/*******************************************************************************
 * Main inproc transport functions
 */

}
