#include "server/server_worker.hpp"

erpc::ServerWorker::ServerWorker(std::vector<erpc::MethodBase *> methods,
                                 TCPTransport *worker)
    : m_worker_thread(workerStub, 5), methods(methods), m_worker(worker) {
  snprintf(TAG, sizeof(TAG) - 1, "worker %" PRIin_port_t, m_worker->m_port);
  m_worker_thread.setName(TAG);
}
erpc::ServerWorker::~ServerWorker() {
  m_method->destroyMsg(m_worker->to_recv_msg, m_worker->to_send_msg);
  delete m_worker;
}

rpc_status erpc::ServerWorker::runInternal(void) {
  rpc_status err;
  myrpc_Meta *req_md = m_worker->to_recv_md;
  myrpc_Meta *rsp_md = m_worker->to_send_md;
  const pb_msgdesc_t **input_desc = &m_worker->recv_desc;
  const pb_msgdesc_t **output_desc = &m_worker->send_desc;
  void **input_msg = &m_worker->to_recv_msg;
  void **output_msg = &m_worker->to_send_msg;
  // get data frame
  CHECK_STATUS(m_worker->receiveFrame(), err);
  // LOGI(TAG, "receiveFrame()");
  
  // recv inital metadata
  CHECK_STATUS(m_worker->recv_inital_md(), err);
  // LOGI(TAG, "recv_inital_md()");

  // find the method
  CHECK_STATUS(findServiceByMetadata(req_md), err);
  // LOGI(TAG, "findServiceByMetadata()");

  // filled message desc, so that we can receive message
  m_method->filledMsgDesc(input_desc, input_msg, output_desc, output_msg);
  // LOGI(TAG, "filledMsgDesc()");

  // recv messgae
  CHECK_STATUS(m_worker->recv_msg(), err);
  // LOGI(TAG, "recv_msg()");

  // recv trailing metadata
  CHECK_STATUS(m_worker->recv_trailing_md(), err);
  // LOGI(TAG, "recv_trailing_md()");

  // call method and filled the meta.
  err = callMethodByMetadata(req_md, rsp_md, m_worker->to_recv_msg,
                             m_worker->to_send_msg);

  // send initial metadata
  CHECK_STATUS(m_worker->send_inital_md(), err);
  // LOGI(TAG, "send_inital_md()");

  // send msg
  CHECK_STATUS(m_worker->send_msg(), err);
  // LOGI(TAG, "send_msg()");

  // send trailing metadata and the whole frame
  rsp_md->has_status = true;
  rsp_md->status = err;
  CHECK_STATUS(m_worker->send_trailing_md(), err);
  // LOGI(TAG, "send_trailing_md()");
done:
  if (err != rpc_status::Success) {
    LOGW(TAG, "RPC Call Failed Because: %s", StatusToString(err));
    if (err == rpc_status::UnknownService) {
      LOGW(TAG, "Request Service: %s", req_md->path);
      LOGE(TAG, "Available Services:");
      for_each(methods.begin(), methods.end(), [this](MethodBase *method) {
        LOGW(TAG, "Service: %s", method->getPath());
      });
    }
    m_worker->close();
  } else {
    LOGI(TAG, "RPC Call Success\n");
  }
  resetBuffers();
  return err;
}
rpc_status erpc::ServerWorker::resetBuffers(void) {
  if (m_method)
    m_method->destroyMsg(m_worker->to_recv_msg, m_worker->to_send_msg);
  m_method = NULL;
  return m_worker->resetBuffers();
}
rpc_status erpc::ServerWorker::findServiceByMetadata(myrpc_Meta *req) {
  auto iter_method = std::find_if(
      methods.begin(), methods.end(), [&](MethodBase *method) -> bool {
        return !strcmp(method->getPath(), req->path);
      });
  if (iter_method == methods.end()) {
    m_method = NULL;
    return rpc_status::UnknownService;
  } else {
    m_method = *iter_method;
  }
  LOGI(this->TAG, "service `%s`", m_method->getPath());
  return rpc_status::Success;
}
rpc_status erpc::ServerWorker::callMethodByMetadata(myrpc_Meta *req,
                                                    myrpc_Meta *rsp,
                                                    void *input, void *output) {
  rpc_status err;
  rsp->seq = req->seq;
  rsp->version = req->version;
  rsp->path = req->path;
  rsp->has_status = true;
  rsp->has_send_end = false;
  if (!m_method) {
    LOGE(TAG, "Unknown service but called");
    rsp->status = rpc_status::UnknownService;
    return rpc_status::UnknownService;
  }
  err = m_method->handleInvocation(input, output);
  rsp->status = err;
  return err;
}

void erpc::ServerWorker::workerStub(void *arg) {
  rpc_status err = rpc_status::Success;
  ServerWorker *This = reinterpret_cast<ServerWorker *>(arg);

  if (This != NULL) {
    while (err == rpc_status::Success) {
      err = This->runInternal();
      LOGI(This->TAG, "runInternal return\n");
    }
  }
  LOGI(This->TAG, "work done\n");
}

void erpc::ServerWorker::start() { m_worker_thread.start(this); }
