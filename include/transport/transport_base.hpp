#ifndef TRANSPORT_BASE_H
#define TRANSPORT_BASE_H

#include "rpc_status.hpp"
#include "port/port.h"
#include "codec/message_buffer.hpp"
#include "port/threading/port_threading.h"


/*!
 * @addtogroup infra_transport
 * @{
 * @file
 */

////////////////////////////////////////////////////////////////////////////////
// Classes
////////////////////////////////////////////////////////////////////////////////

namespace erpc {

/*!
 * @brief Abstract interface for transport layer.
 *
 * @ingroup infra_transport
 */
class Transport
{
public:
    /*!
     * @brief Constructor.
     */
    Transport(void) {}

    /*!
     * @brief Transport destructor
     */
    virtual ~Transport(void) {}

    /*!
     * @brief Prototype for receiving message.
     *
     * Each transport layer need define this function.
     *
     * @param[out] message Will return pointer to received message buffer.
     *
     * @return based on receive implementation.
     */
    virtual rpc_status receive(uint8_t *data, uint32_t size) = 0;

    /*!
     * @brief Prototype for send message.
     *
     * Each transport layer need define this function.
     *
     * @param[in] message Pass message buffer to send.
     *
     * @return based on send implementation.
     */
    virtual rpc_status send(const uint8_t *data, uint32_t size) = 0;
};

/*!
 * @brief Abstract interface for transport factory.
 *
 * @ingroup infra_transport
 */
class TransportFactory
{
public:
    /*!
     * @brief Constructor.
     */
    TransportFactory(void) {}
    /*!
     * @brief TransportFactory destructor
     */
    virtual ~TransportFactory(void) {}
    /*!
     * @brief Return created transport object.
     *
     * @return Pointer to created transport object.
     */
    virtual Transport *create(void) = 0;
};

} // namespace erpc

/*! @} */

#endif // TRANSPORT_BASE_H
