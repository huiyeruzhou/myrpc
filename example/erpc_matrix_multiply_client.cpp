/*
 * You can write copyrights rules here. These rules will be copied into the outputs.
 */

/*
 * Generated by erpcgen 1.9.1 on Thu Mar 30 20:15:37 2023.
 *
 * AUTOGENERATED - DO NOT EDIT
 */


#include "client/rpc_client.h"
#include "port/port.h"
#include <err.h>
#include "codec/codec_base.hpp"
extern "C"
{
#include "erpc_matrix_multiply.h"
}

#if 10901 != ERPC_VERSION_NUMBER
#error "The generated shim code version is different to the rest of eRPC code."
#endif

using namespace erpc;
using namespace std;



// Constant variable definitions
#pragma weak matrix_size
extern const int32_t matrix_size = 2;


// MatrixMultiplyService interface erpcMatrixMultiply function client shim.
void MatrixMultiplyServiceClient::erpcMatrixMultiply(Matrix matrix1, Matrix matrix2, Matrix result_matrix)
{
    rpc_status err = rpc_status::Success;



    // Get a new request.
    RequestContext request = createRequest(false);

    // Encode the request.
    Codec * codec = request.getCodec();

    if (codec == NULL)
    {
        err = rpc_status::MemoryError;
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
        performRequest(request);

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
    releaseRequest(request);

    // Invoke error handler callback function
    callErrorHandler(err, kMatrixMultiplyService_erpcMatrixMultiply_id);



    return;
}

// MatrixMultiplyService interface erpctest function client shim.
void MatrixMultiplyServiceClient::erpctest(int32_t num1, int32_t num2, int32_t * ret)
{
    rpc_status err = rpc_status::Success;



    // Get a new request.
    RequestContext request = createRequest(false);

    // Encode the request.
    Codec * codec = request.getCodec();

    if (codec == NULL)
    {
        err = rpc_status::MemoryError;
    }
    else
    {
        codec->startWriteMessage(kInvocationMessage, kMatrixMultiplyService_service_id, kMatrixMultiplyService_erpctest_id, request.getSequence());

        codec->write(num1);

        codec->write(num2);

        // Send message to server
        // Codec status is checked inside this function.
        performRequest(request);

        codec->read(ret);

        err = codec->getStatus();
    }

    // Dispose of the request.
    releaseRequest(request);

    // Invoke error handler callback function
    callErrorHandler(err, kMatrixMultiplyService_erpctest_id);



    return;
}
