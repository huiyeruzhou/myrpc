#ifndef CLIENT_RPC_CLIENT_HPP_
#define CLIENT_RPC_CLIENT_HPP_

#include <netdb.h>

#include "codec/message_buffer.hpp"
#include "port/port.h"
#include "rpc_base.hpp"
#include "rpc_status.hpp"
#ifdef CONFIG_HAS_POSIX
#include <arpa/inet.h>
#include <signal.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#endif

namespace erpc {

/*!
 * @brief Base client implementation.
 *
 * @ingroup infra_client
 */
class Client : public CSBase {
 public:
  /*!
   * @brief Constructor.
   *
   * This function initializes object attributes.
   */
  Client(const char *host, uint16_t port);

  /*!
   * @brief Client destructor
   */
  virtual ~Client(void);

  /*!
   * @brief This function connect client to the server.
   *
   * @retval Success When client connected successfully.
   * @retval Fail When client doesn't connected successfully.
   */
  virtual rpc_status open(void);

  virtual rpc_status close(void);

 protected:
  virtual rpc_status performRequest(char *path, const pb_msgdesc_t *req_desc,
                                    void *req, const pb_msgdesc_t *rsp_desc,
                                    void *rsp);

 private:
  uint32_t m_sequence;                     //!< Sequence number.
  Client(const Client &other);             //!< Disable copy ctor.
  Client &operator=(const Client &other);  //!< Disable copy ctor.
};

}  // namespace erpc

#endif  // CLIENT_RPC_CLIENT_HPP_
