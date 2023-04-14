/*
 * You can write copyrights rules here. These rules will be copied into the outputs.
 */

/*
 * Generated by erpcgen 1.9.1 on Tue Mar 28 10:14:18 2023.
 *
 * AUTOGENERATED - DO NOT EDIT
 */


#if !defined(_qaq_server_h_)
#define _qaq_server_h_

#ifdef __cplusplus
#include "server_base.hpp"
#include "codec_base.hpp"
extern "C"
{
#include "qaq.h"
#include <stdint.h>
#include <stdbool.h>
}

#if 10901 != ERPC_VERSION_NUMBER
#error "The generated shim code version is different to the rest of eRPC code."
#endif


/*!
 * @brief Service subclass for aaa.
 */
class aaa_service : public erpc::Service
{
public:
    aaa_service() : Service(kaaa_service_id) {}

    /*! @brief Call the correct server shim based on method unique ID. */
    virtual rpc_status handleInvocation(uint32_t methodId, uint32_t sequence, erpc::Codec * codec, erpc::MessageBufferFactory *messageFactory);

private:
    /*! @brief Server shim for erpctest of aaa interface. */
    rpc_status erpctest_shim(erpc::Codec * codec, erpc::MessageBufferFactory *messageFactory, uint32_t sequence);
};

extern "C" {
#else
#include "qaq.h"
#endif // __cplusplus

typedef void * erpc_service_t;

/*! @brief Return aaa_service service object. */
erpc_service_t create_aaa_service(void);

/*! @brief Destroy aaa_service service object. */
void destroy_aaa_service(erpc_service_t service);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // _qaq_server_h_