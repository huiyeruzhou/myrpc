#ifndef SERVER_SERVER_BASE_HPP_
#define SERVER_SERVER_BASE_HPP_

#include <vector>
#include <memory>

#include "port/port.h"
#include "rpc_base.hpp"
#include "server/service.hpp"

namespace erpc {

class Server : public CSBase {
 public:
  Server(const char *host, uint16_t port)
      : CSBase(host, port), methods(new MethodVector()) {
    // LOGE("memory", "server construct methods=%ld", methods.use_count());
  }

  virtual ~Server(void);

  /*!
   * @brief Add service.
   *
   * @param[in] service Service to use.
   */
  void addService(Service *service);

  /*!
   * @brief Remove service.
   *
   * @param[in] service Service to remove.
   */
  void removeService(Service *service);

  /*!
   * @brief This function will start the network thread to accept new
   * connections.
   */
  virtual rpc_status open(void) = 0;

  /*!
   * @brief This function will stop the network thread, but it can restart by
   calling open() again. The worker threads will continue to process RPC
   requests from existing connections.
   */
  virtual rpc_status run(void) = 0;

  /*!
   * @brief This function stop the server.Which means the server will no longer
   * accept new connections. Its worker will continue to process requests but
   * the network thread will exit after next return from accept() and the server
   * will be destroyed.
   */
  virtual void stop(void) = 0;

 protected:
  std::shared_ptr<MethodVector> methods;

 private:
  // Disable copy ctor.
  Server(const Server &other);            /*!< Disable copy ctor. */
  Server &operator=(const Server &other); /*!< Disable copy ctor. */
};

}  // namespace erpc

#endif  // SERVER_SERVER_BASE_HPP_
