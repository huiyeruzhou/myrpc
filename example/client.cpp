#include "idl.pb.h"

#include <stdio.h>
#include <unistd.h>


int main(int argc,char** argv)
{

    /* MessageBufferFactory initialization */
    erpc::MessageBufferFactory *message_buffer_factory = new erpc::MessageBufferFactory();

    /* eRPC client side initialization */
    // MatrixMultiplyServiceClient *client = new MatrixMultiplyServiceClient("192.168.0.101", 12345, message_buffer_factory);
    // MatrixMultiplyServiceClient *client = new MatrixMultiplyServiceClient("192.168.1.6", 12345, message_buffer_factory);
    auto *client = new myrpc_MatrixMultiplyService_Client("localhost", 12345, message_buffer_factory);

    /* code */
    int32_t ret = 0;
    if (rpc_status::Success != client->open()) return -1;
    for (;;)
    {
        myrpc_InputTest req;
        myrpc_OutputTest rsp;
        req.num1 = 1;
        req.num2 = 2;
        client->myrpctest(&req, &rsp);
        printf("response: %" PRId32 "\n", rsp.ret);
    }
    // printMatrix(result_matrix);
    /* other code like print result matrix */
    
    return 0;
}
// arch linux

//kde
