/*
 * You can write copyrights rules here. These rules will be copied into the outputs.
 */

/*
 * Generated by erpcgen 1.9.1 on Mon Dec  5 04:28:44 2022.
 *
 * AUTOGENERATED - DO NOT EDIT
 */


#include "erpc_client_manager.h"
#if ERPC_ALLOCATION_POLICY == ERPC_ALLOCATION_POLICY_DYNAMIC
#include "erpc_port.h"
#endif
#include "erpc_codec.hpp"
extern "C"
{
#include "erpc_matrix_multiply.h"
}

#if 10901 != ERPC_VERSION_NUMBER
#error "The generated shim code version is different to the rest of eRPC code."
#endif

using namespace erpc;
using namespace std;

extern ClientManager *g_client;


// Constant variable definitions
#pragma weak matrix_size
extern const int32_t matrix_size = 2;


// MatrixMultiplyService interface erpcMatrixMultiply function client shim.
void erpcMatrixMultiply(Matrix matrix1, Matrix matrix2, Matrix result_matrix)
{
    erpc_status_t err = kErpcStatus_Success;


#if ERPC_PRE_POST_ACTION
    pre_post_action_cb preCB = g_client->getPreCB();
    if (preCB)
    {
        preCB();
    }
#endif

    // Get a new request.
    RequestContext request = g_client->createRequest(false);

    // Encode the request.
    Codec * codec = request.getCodec();

    if (codec == NULL)
    {
        err = kErpcStatus_MemoryError;
    }
    else
    {
        codec->startWriteMessage(kInvocationMessage, kMatrixMultiplyService_service_id, kMatrixMultiplyService_erpcMatrixMultiply_id, request.getSequence());

        for (uint32_t arrayCount0 = 0U; arrayCount0 < 2U; ++arrayCount0)
        {
            for (uint32_t arrayCount1 = 0U; arrayCount1 < 2U; ++arrayCount1)
            {
                codec->write(matrix1[arrayCount0][arrayCount1]);
            }
        }

        for (uint32_t arrayCount0 = 0U; arrayCount0 < 2U; ++arrayCount0)
        {
            for (uint32_t arrayCount1 = 0U; arrayCount1 < 2U; ++arrayCount1)
            {
                codec->write(matrix2[arrayCount0][arrayCount1]);
            }
        }

        // Send message to server
        // Codec status is checked inside this function.
        g_client->performRequest(request);

        for (uint32_t arrayCount0 = 0U; arrayCount0 < 2U; ++arrayCount0)
        {
            for (uint32_t arrayCount1 = 0U; arrayCount1 < 2U; ++arrayCount1)
            {
                codec->read(&result_matrix[arrayCount0][arrayCount1]);
            }
        }

        err = codec->getStatus();
    }

    // Dispose of the request.
    g_client->releaseRequest(request);

    // Invoke error handler callback function
    g_client->callErrorHandler(err, kMatrixMultiplyService_erpcMatrixMultiply_id);

#if ERPC_PRE_POST_ACTION
    pre_post_action_cb postCB = g_client->getPostCB();
    if (postCB)
    {
        postCB();
    }
#endif


    return;
}

// MatrixMultiplyService interface erpctest function client shim.
void erpctest(int32_t num1, int32_t num2, int32_t * ret)
{
    erpc_status_t err = kErpcStatus_Success;


#if ERPC_PRE_POST_ACTION
    pre_post_action_cb preCB = g_client->getPreCB();
    if (preCB)
    {
        preCB();
    }
#endif

    // Get a new request.
    RequestContext request = g_client->createRequest(false);

    // Encode the request.
    Codec * codec = request.getCodec();

    if (codec == NULL)
    {
        err = kErpcStatus_MemoryError;
    }
    else
    {
        codec->startWriteMessage(kInvocationMessage, kMatrixMultiplyService_service_id, kMatrixMultiplyService_erpctest_id, request.getSequence());

        codec->write(num1);

        codec->write(num2);

        // Send message to server
        // Codec status is checked inside this function.
        g_client->performRequest(request);

        codec->read(ret);

        err = codec->getStatus();
    }

    // Dispose of the request.
    g_client->releaseRequest(request);

    // Invoke error handler callback function
    g_client->callErrorHandler(err, kMatrixMultiplyService_erpctest_id);

#if ERPC_PRE_POST_ACTION
    pre_post_action_cb postCB = g_client->getPostCB();
    if (postCB)
    {
        postCB();
    }
#endif


    return;
}
