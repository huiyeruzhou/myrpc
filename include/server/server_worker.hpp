#ifndef SERVER_SERVER_WORKER_HPP_
#define SERVER_SERVER_WORKER_HPP_

extern "C" {
#include <unistd.h>
}

#include <cstdio>
#include <vector>
#include <memory>

#include "codec/meta.pb.h"
#include "port/port.h"
#include "server/service.hpp"
#include "transport/tcp_transport.hpp"

#ifdef CHECK_STATUS
#undef CHECK_STATUS
#endif
#define CHECK_STATUS(status, err)                           \
  if (((err) = (status)) != rpc_status::Success) {          \
    LOGE(TAG, "Error occurred: %s", StatusToString((err))); \
    goto done;                                              \
  }
namespace erpc {
class ServerWorker {
 public:
  ServerWorker(std::shared_ptr<MethodVector> methods, TCPTransport *worker);
  ~ServerWorker();
  rpc_status runInternal(void);
  rpc_status resetBuffers(void);
  rpc_status findServiceByMetadata(myrpc_Meta *req);
  rpc_status callMethodByMetadata(myrpc_Meta *req, myrpc_Meta *esp, void *input,
                                  void *output);
  static void workerStub(void *arg);
  void start(void);

 private:
  Thread m_worker_thread;
#if ERPC_THREADS_IS(PTHREADS)
  char TAG[CONFIG_MAX_PTHREAD_NAME_LEN];
#elif ERPC_THREADS_IS(FREERTOS)
  char TAG[configMAX_TASK_NAME_LEN];
#endif
  std::shared_ptr<MethodBase> m_method = NULL;
  std::shared_ptr<MethodVector> methods;
  TCPTransport *m_worker;  //!< Worker to do transport
};

}  // namespace erpc

#endif  // SERVER_SERVER_WORKER_HPP_
