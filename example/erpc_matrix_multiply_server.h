/*
 * You can write copyrights rules here. These rules will be copied into the outputs.
 */

/*
 * Generated by erpcgen 1.9.1 on Thu Mar 30 20:15:37 2023.
 *
 * AUTOGENERATED - DO NOT EDIT
 */


#if !defined(_erpc_matrix_multiply_server_h_)
#define _erpc_matrix_multiply_server_h_

#ifdef __cplusplus
#include "server/server_base.hpp"
#include "server/simple_server.hpp"
#include "codec/codec_base.hpp"
extern "C"
{
#include "erpc_matrix_multiply.h"
#include <stdint.h>
#include <stdbool.h>
}

#if 10901 != ERPC_VERSION_NUMBER
#error "The generated shim code version is different to the rest of eRPC code."
#endif


/*!
 * @brief Service subclass for MatrixMultiplyService.
 */
class MatrixMultiplyService_service : public erpc::Service
{
public:
    MatrixMultiplyService_service() : Service(kMatrixMultiplyService_service_id) {}

    /*! @brief Call the correct server shim based on method unique ID. */
    virtual rpc_status handleInvocation(uint32_t methodId, uint32_t sequence, erpc::Codec * codec, erpc::MessageBufferFactory *messageFactory);

private:
    /*! @brief Server shim for erpcMatrixMultiply of MatrixMultiplyService interface. */
    rpc_status erpcMatrixMultiply_shim(erpc::Codec * codec, erpc::MessageBufferFactory *messageFactory, uint32_t sequence);

    /*! @brief Server shim for erpctest of MatrixMultiplyService interface. */
    rpc_status erpctest_shim(erpc::Codec * codec, erpc::MessageBufferFactory *messageFactory, uint32_t sequence);
};

extern "C" {
#else
#include "erpc_matrix_multiply.h"
#endif // __cplusplus

typedef void * erpc_service_t;

/*! @brief Return MatrixMultiplyService_service service object. */
erpc_service_t create_MatrixMultiplyService_service(void);

/*! @brief Destroy MatrixMultiplyService_service service object. */
void destroy_MatrixMultiplyService_service(erpc_service_t service);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // _erpc_matrix_multiply_server_h_
