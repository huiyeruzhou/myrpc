#ifndef _TRANSPROT_H
#define _TRANSPROT_H
#include "port/port.h"
#include <stdint.h>
#include "server/simple_server.hpp"
#include "codec/meta.pb.h"
#include "codec/message_buffer.hpp"
#include "codec/nanopb_codec.hpp"
#include "rpc_status.hpp"
#include <functional>
namespace erpc {
    typedef rpc_status grpc_error_handle;
    class Timestamp {
    public:
        static constexpr Timestamp InfFuture() {
            return Timestamp(INT64_MAX);
        }
        constexpr bool operator!=(Timestamp other) const {
            return millis_ != other.millis_;
        }
    private:
        friend class grpc_metadata_batch;
        explicit constexpr Timestamp(int64_t millis): millis_(millis) {}
        int64_t millis_ = 0;
    };

    class grpc_metadata_batch {
    public:
        void Clear() { valid_ = false; }
        void SetTimeout(const Timestamp &ts) {
            metadata.timeout = ts.millis_;
        }
        
        Timestamp get_timeout() {
            return Timestamp(metadata.timeout);
        }
    private:
        bool valid_ = true;
        myrpc_Meta metadata;
    };


    /** gRPC Callback definition.
    *
    * \param arg Arbitrary input.
    * \param error rpc_status::Success if no error occurred, otherwise some grpc_error
    *              describing what went wrong.
    *              Error contract: it is not the cb's job to unref this error;
    *              the closure scheduler will do that after the cb returns */
    typedef void (*grpc_iomgr_cb_func)(void *arg, rpc_status error);

    struct grpc_transport_one_way_stats {
        uint64_t framing_bytes = 0;
        uint64_t data_bytes = 0;
        uint64_t header_bytes = 0;
    };
    struct grpc_handler_private_op_data {
        void *extra_arg = nullptr;
        grpc_closure closure;
        grpc_handler_private_op_data() { memset(&closure, 0, sizeof(closure)); }
    };
    struct grpc_transport_stream_stats {
        grpc_transport_one_way_stats incoming;
        grpc_transport_one_way_stats outgoing;
    };

    struct grpc_closure {
        /** Once queued, next indicates the next queued closure; before then, scratch
         *  space */
         // grpc_closure *next;


         /** Bound callback. */
        grpc_iomgr_cb_func cb;

        /** Arguments to be passed to "cb". */
        void *cb_arg;

        /** Once queued, the result of the closure. Before then: scratch space */
        rpc_status *error_data;

        void run(rpc_status error) {
            cb(cb_arg, error);
        }
    };
    inline grpc_closure *grpc_closure_init(grpc_closure *closure,
        grpc_iomgr_cb_func cb, void *cb_arg) {
        closure->cb = cb;
        closure->cb_arg = cb_arg;
        *closure->error_data = rpc_status::Success;
        return closure;
    }

    struct grpc_transport_stream_op_batch_payload {
        explicit grpc_transport_stream_op_batch_payload(
            // grpc_call_context_element *context)
            // : context(context) {
        ) {
        }
        struct {
            grpc_metadata_batch *send_initial_metadata = nullptr;
            // If non-NULL, will be set by the transport to the peer string (a char*).
            // The transport retains ownership of the string.
            // Note: This pointer may be used by the transport after the
            // send_initial_metadata op is completed.  It must remain valid
            // until the call is destroyed.
            intptr_t *peer_string = nullptr;
        } send_initial_metadata;

        struct {
            grpc_metadata_batch *send_trailing_metadata = nullptr;
            // Set by the transport to true if the stream successfully wrote the
            // trailing metadata. If this is not set but there was a send trailing
            // metadata op present, this can indicate that a server call can be marked
            // as  a cancellation (since the stream was write-closed before status could
            // be delivered).
            bool *sent = nullptr;
        } send_trailing_metadata;

        struct {
            // The transport (or a filter that decides to return a failure before
            // the op gets down to the transport) takes ownership.
            // The batch's on_complete will not be called until after the byte
            // stream is orphaned.
            MessageBufferList *send_message;
            uint32_t flags = 0;
            // Set by the transport if the stream has been closed for writes. If this
            // is set and send message op is present, we set the operation to be a
            // failure without sending a cancel OP down the stack. This is so that the
            // status of the call does not get overwritten by the Cancel OP, which would
            // be especially problematic if we had received a valid status from the
            // server.
            // For send_initial_metadata, it is fine for the status to be overwritten
            // because at that point, the client will not have received a status.
            // For send_trailing_metadata, we might overwrite the status if we have
            // non-zero metadata to send. This is fine because the API does not allow
            // the client to send trailing metadata.
            bool stream_write_closed = false;
        } send_message;

        struct {
            grpc_metadata_batch *recv_initial_metadata = nullptr;
            /** Should be enqueued when initial metadata is ready to be processed. */
            grpc_closure *recv_initial_metadata_ready = nullptr;
            // If not NULL, will be set to true if trailing metadata is
            // immediately available. This may be a signal that we received a
            // Trailers-Only response. The retry filter checks this to know whether to
            // defer the decision to commit the call or not. The C++ callback API also
            // uses this to set the success flag of OnReadInitialMetadataDone()
            // callback.
            bool *trailing_metadata_available = nullptr;
            // If non-NULL, will be set by the transport to the peer string (a char*).
            // The transport retains ownership of the string.
            // Note: This pointer may be used by the transport after the
            // recv_initial_metadata op is completed.  It must remain valid
            // until the call is destroyed.
            intptr_t *peer_string = nullptr;
        } recv_initial_metadata;

        struct {
            // Will be set by the transport to point to the byte stream containing a
            // received message. Will be nullopt if trailing metadata is received
            // instead of a message.
            MessageBufferList *recv_message = nullptr;
            uint32_t *flags = nullptr;
            // Was this recv_message failed for reasons other than a clean end-of-stream
            bool *call_failed_before_recv_message = nullptr;
            /** Should be enqueued when one message is ready to be processed. */
            grpc_closure *recv_message_ready = nullptr;
        } recv_message;

        struct {
            grpc_metadata_batch *recv_trailing_metadata = nullptr;
            grpc_transport_stream_stats *collect_stats = nullptr;
            /** Should be enqueued when trailing metadata is ready to be processed. */
            grpc_closure *recv_trailing_metadata_ready = nullptr;
        } recv_trailing_metadata;

        /** Forcefully close this stream.
            The HTTP2 semantics should be:
            - server side: if cancel_error has
           grpc_core::StatusIntProperty::kRpcStatus, and trailing metadata has not
           been sent, send trailing metadata with status and message from cancel_error
           (use grpc_error_get_status) followed by a RST_STREAM with
           error=GRPC_CHTTP2_NO_ERROR to force a full close
            - at all other times: use grpc_error_get_status to get a status code, and
              convert to a HTTP2 error code using
              grpc_chttp2_grpc_status_to_http2_error. Send a RST_STREAM with this
              error. */
        struct {
            // Error contract: the transport that gets this op must cause cancel_error
            //                 to be unref'ed after processing it
            grpc_error_handle cancel_error;
        } cancel_stream;

        /* Indexes correspond to grpc_context_index enum values */
        // grpc_call_context_element *context;
    };

    struct grpc_transport_stream_op_batch {
        grpc_transport_stream_op_batch()
            : send_initial_metadata(false),
            send_trailing_metadata(false),
            send_message(false),
            recv_initial_metadata(false),
            recv_message(false),
            recv_trailing_metadata(false),
            cancel_stream(false),
            is_traced(false) {
        }

        /** Should be scheduled when all of the non-recv operations in the batch
            are complete.

            The recv ops (recv_initial_metadata, recv_message, and
            recv_trailing_metadata) each have their own callbacks.  If a batch
            contains both recv ops and non-recv ops, on_complete should be
            scheduled as soon as the non-recv ops are complete, regardless of
            whether or not the recv ops are complete.  If a batch contains
            only recv ops, on_complete can be null. */
        grpc_closure *on_complete = nullptr;

        /** Values for the stream op (fields set are determined by flags above) */
        grpc_transport_stream_op_batch_payload *payload = nullptr;

        /** Send initial metadata to the peer, from the provided metadata batch. */
        bool send_initial_metadata : 1;

        /** Send trailing metadata to the peer, from the provided metadata batch. */
        bool send_trailing_metadata : 1;

        /** Send message data to the peer, from the provided byte stream. */
        bool send_message : 1;

        /** Receive initial metadata from the stream, into provided metadata batch. */
        bool recv_initial_metadata : 1;

        /** Receive message data from the stream, into provided byte stream. */
        bool recv_message : 1;

        /** Receive trailing metadata from the stream, into provided metadata batch.
         */
        bool recv_trailing_metadata : 1;

        /** Cancel this stream with the provided error */
        bool cancel_stream : 1;

        /** Is this stream traced */
        bool is_traced : 1;

        /***************************************************************************
         * remaining fields are initialized and used at the discretion of the
         * current handler of the op */

        grpc_handler_private_op_data handler_private;
    };
    /** Transport op: a set of operations to perform on a transport as a whole */

    struct grpc_transport_op {
        /** Called when processing of this op is done. */
        grpc_closure *on_consumed = nullptr;
        /** connectivity monitoring - set connectivity_state to NULL to unsubscribe */
        erpc::Server *start_connectivity_watch;
        // grpc_connectivity_state start_connectivity_watch_state = GRPC_CHANNEL_IDLE;
        // grpc_core::ConnectivityStateWatcherInterface *stop_connectivity_watch =
        //     nullptr;
        /** should the transport be disconnected
         * Error contract: the transport that gets this op must cause
         *                 disconnect_with_error to be unref'ed after processing it */
        grpc_error_handle disconnect_with_error;
        /** what should the goaway contain?
         * Error contract: the transport that gets this op must cause
         *                 goaway_error to be unref'ed after processing it */
        grpc_error_handle goaway_error;
        /** set the callback for accepting new streams;
            this is a permanent callback, unlike the other one-shot closures.
            If true, the callback is set to set_accept_stream_fn, with its
            user_data argument set to set_accept_stream_user_data */
        bool set_accept_stream = false;
        void (*set_accept_stream_fn)(void *user_data, const void *server_data) = nullptr;
        void *set_accept_stream_user_data = nullptr;
        // /** set the callback for accepting new streams based upon promises;
        //     this is a permanent callback, unlike the other one-shot closures.
        //     If true, the callback is set to set_make_promise_fn, with its
        //     user_data argument set to set_make_promise_data */
        // bool set_make_promise = false;
        // void (*set_make_promise_fn)(void *user_data, grpc_transport *transport,
        //     const void *server_data) = nullptr;
        // void *set_make_promise_user_data = nullptr;
        /** send a ping, if either on_initiate or on_ack is not NULL */
        // struct {
        //     /** Ping may be delayed by the transport, on_initiate callback will be
        //         called when the ping is actually being sent. */
        //     grpc_closure *on_initiate = nullptr;
        //     /** Called when the ping ack is received */
        //     grpc_closure *on_ack = nullptr;
        // } send_ping;
        // If true, will reset the channel's connection backoff.
        bool reset_connect_backoff = false;

        /***************************************************************************
         * remaining fields are initialized and used at the discretion of the
         * transport implementation */

        grpc_handler_private_op_data handler_private;
    };

    struct inproc_stream {
        inproc_stream(inproc_transport *t,
            const void *server_data);
        inproc_transport *t;

        grpc_metadata_batch to_read_initial_md;
        bool to_read_initial_md_filled = false;
        grpc_metadata_batch to_read_trailing_md;
        bool to_read_trailing_md_filled = false;
        bool ops_needed = false;
        // Write buffer used only during gap at init time when client-side
        // stream is set up but server side stream is not yet set up
        grpc_metadata_batch write_buffer_initial_md;
        bool write_buffer_initial_md_filled = false;
        Timestamp write_buffer_deadline =
            Timestamp::InfFuture();
        grpc_metadata_batch write_buffer_trailing_md;
        bool write_buffer_trailing_md_filled = false;
        rpc_status write_buffer_cancel_error;

        bool other_side_closed = false;               // won't talk anymore
        bool write_buffer_other_side_closed = false;  // on hold

        grpc_transport_stream_op_batch *send_message_op = nullptr;
        grpc_transport_stream_op_batch *send_trailing_md_op = nullptr;
        grpc_transport_stream_op_batch *recv_initial_md_op = nullptr;
        grpc_transport_stream_op_batch *recv_message_op = nullptr;
        grpc_transport_stream_op_batch *recv_trailing_md_op = nullptr;

        bool initial_md_sent = false;
        bool trailing_md_sent = false;
        bool initial_md_recvd = false;
        bool trailing_md_recvd = false;
        // The following tracks if the server-side only pretends to have received
        // trailing metadata since it no longer cares about the RPC. If that is the
        // case, it is still ok for the client to send trailing metadata (in which
        // case it will be ignored).
        bool trailing_md_recvd_implicit_only = false;

        bool closed = false;

        rpc_status cancel_self_error;
        rpc_status cancel_other_error;

        Timestamp deadline = Timestamp::InfFuture();

        bool listed = true;
        inproc_stream *stream_list_prev;
        inproc_stream *stream_list_next;

        const char *m_host;    /*!< Specify the host name or IP address of the computer. */
        uint16_t m_port = 0;       /*!< Specify the listening port number. */
        int m_sockfd = -1;

        TCPWorker *m_worker;
        NanopbCodec *m_odec;
    };
    struct inproc_transport {
    public:
        inproc_transport(bool is_client)
            : 
            is_client(is_client)
        {
        }
        ~inproc_transport() {
        }
        // shared_mu* mu;
        bool is_client;
        //    ConnectivityStateTracker state_tracker;
        void (*accept_stream_cb)(void *user_data, const void *server_data);
        void *accept_stream_data;
        bool is_closed = false;
        Server *watcher = nullptr;
        inproc_transport *other_side;
        inproc_stream *stream_list = nullptr;
    };
}
#endif  //TRANSPORT_H