/*
 * Copyright (c) 2014-2016, Freescale Semiconductor, Inc.
 * Copyright 2016-2017 NXP
 * Copyright 2020-2021 ACRIOS Systems s.r.o.
 * All rights reserved.
 *
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef RPC_CLIENT_H
#define RPC_CLIENT_H

#include "rpc_status.hpp"
#include "port/port.h"
#include "rpc_base.hpp"
#include "codec/message_buffer.hpp"

#include <netdb.h>


/*!
 * @addtogroup infra_client
 * @{
 * @file
 */


#ifdef __cplusplus
extern "C" {
#endif
    typedef void (*client_error_handler_t)(rpc_status err,
                                           uint32_t functionID); /*!< eRPC error handler function type. */
#ifdef __cplusplus
}
#endif


////////////////////////////////////////////////////////////////////////////////
// Classes
////////////////////////////////////////////////////////////////////////////////

namespace erpc {
    class RequestContext;
#if ERPC_NESTED_CALLS
class Server;
#endif

/*!
 * @brief Base client implementation.
 *
 * @ingroup infra_client
 */
class Client : public CSBase
{
public:
    /*!
     * @brief Constructor.
     *
     * This function initializes object attributes.
     */
    Client(const char *host, uint16_t port, MessageBufferFactory *messageFactory);

    /*!
     * @brief Client destructor
     */
    virtual ~Client(void);

    /*!
     * @brief This function creates request context.
     *
     * @param[in] isOneway True if need send data only, else false.
     */
    virtual RequestContext createRequest(bool isOneway);

    /*!
     * @brief This function performs request.
     *
     * @param[in] request Request context to perform.
     */
    virtual void performRequest(RequestContext &request);


    /*!
     * @brief This function sets error handler function for infrastructure errors.
     *
     * @param[in] error_handler Pointer to error handler function.
     */
    void setErrorHandler(client_error_handler_t error_handler) { m_errorHandler = error_handler; }

    /*!
     * @brief This function calls error handler callback function with given status.
     *
     * Function is called in client shim code at the end of function when error occurred.
     *
     * @param[in] err Specify function status at the end of eRPC call.
     * @param[in] functionID Specify eRPC function call.
     */
    void callErrorHandler(rpc_status err, uint32_t functionID);

    /*!
    * @brief This function connect client to the server.
    *
    * @retval Success When client connected successfully.
    * @retval Fail When client doesn't connected successfully.
    */
    virtual rpc_status open(void);

protected:
    uint32_t m_sequence;                    //!< Sequence number.
    client_error_handler_t m_errorHandler;  //!< Pointer to function error handler.

    /*!
     * @brief This function performs request.
     *
     * Should be called in non server context (do not call another eRPC function in server
     * remote call implementation).
     *
     * @param[in] request Request context to perform.
     */
    virtual void performClientRequest(RequestContext &request);

    //! @brief Validate that an incoming message is a reply.
    virtual void verifyReply(RequestContext &request);



private:
    Client(const Client &other);            //!< Disable copy ctor.
    Client &operator=(const Client &other); //!< Disable copy ctor.
};

/*!
 * @brief Encapsulates all information about a request.
 *
 * @ingroup infra_client
 */
class RequestContext
{
public:
    /*!
     * @brief Constructor.
     *
     * This function sets request context attributes.
     *
     * @param[in] sequence Sequence number.
     * @param[in] codec Set in inout codec.
     * @param[in] isOneway Set information if codec is only oneway or bidirectional.
     */
    RequestContext(uint32_t sequence,  bool argIsOneway, MessageBufferFactory *bufferfactory)
    : m_sequence(sequence)
        , m_oneway(argIsOneway)
        , status(Success)
    {
    }

    bool isStatusOk(void) const { return status == Success; }

    rpc_status getStatus(void) const { return status; }

    void updateStatus(rpc_status newStatus) { status = newStatus; }

    /*!
     * @brief Get sequence number (be sure that reply belong to current request).
     *
     * @return Sequence number.
     */
    uint32_t getSequence(void) const { return m_sequence; }

    /*!
     * @brief Returns information if request context is oneway or not.
     *
     * @retval True when request context is oneway direction, else false.
     */
    bool isOneway(void) const { return m_oneway; }

    /*!
     * @brief Set request context to be oneway type (only send data).
     *
     * @return Set request context to be oneway.
     */
    void setIsOneway(bool oneway) { m_oneway = oneway; }

protected:
    uint32_t m_sequence; //!< Sequence number. To be sure that reply belong to current request.
    bool m_oneway;       //!< When true, request context will be oneway type (only send data).
    rpc_status status;   //!< Status of request context.
};

} // namespace erpc

/*! @} */

#endif // RPC_CLIENT_H
