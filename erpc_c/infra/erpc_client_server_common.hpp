/*
 * Copyright 2020 NXP
 * Copyright 2020 ACRIOS Systems s.r.o.
 * All rights reserved.
 *
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _EMBEDDED_RPC__CLIENTSERVERCOMMON_H_
#define _EMBEDDED_RPC__CLIENTSERVERCOMMON_H_

#include "erpc_config_internal.h"
#include "erpc_codec.hpp"


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
 * @brief Common class inheritand by client and server class.
 *
 * @ingroup infra_utility
 */
class ClientServerCommon
{
public:
    /*!
     * @brief ClientServerCommon constructor.
     */
    ClientServerCommon(void):
        m_messageFactory(NULL), m_codecFactory(NULL), m_transport(NULL){};

    /*!
     * @brief ClientServerCommon destructor
     */
    ~ClientServerCommon(void){};

    /*!
     * @brief This function sets message buffer factory to use.
     *
     * @param[in] factory Message buffer factory to use.
     */
    void setMessageBufferFactory(MessageBufferFactory *factory) { m_messageFactory = factory; }

    /*!
     * @brief This function sets codec factory to use.
     *
     * @param[in] factory Codec factory to use.
     */
    void setCodecFactory(CodecFactory *factory) { m_codecFactory = factory; }

    /*!
     * @brief This function sets codec factory to use.
     *
     * @return CodecFactory * Codec factory to use.
     */
    CodecFactory * getCodecFactory(void) { return m_codecFactory; }

    /*!
     * @brief This function sets transport layer to use.
     *
     * It also set messageBufferFactory to the same as in transport layer.
     *
     * @param[in] transport Transport layer to use.
     */
    void setTransport(Transport *transport) { m_transport = transport; }

    /*!
     * @brief This function gets transport instance.
     *
     * @return Transport * Pointer to transport instance.
     */
    Transport * getTransport(void) { return m_transport; }

protected:
    MessageBufferFactory *m_messageFactory; //!< Message buffer factory to use.
    CodecFactory *m_codecFactory;           //!< Codec to use.
    Transport *m_transport;                 //!< Transport layer to use.
};

} // namespace erpc

/*! @} */

#endif // _EMBEDDED_RPC__CLIENTSERVERCOMMON_H_
