#include "idl.pb.hpp"

#include <stdio.h>
#include <unistd.h>


int main(int argc, char **argv) {

    /* MessageBufferFactory initialization */


    /* eRPC client side initialization */
    // MatrixMultiplyServiceClient *client = new MatrixMultiplyServiceClient("192.168.0.101", 12345, message_buffer_factory);
    // MatrixMultiplyServiceClient *client = new MatrixMultiplyServiceClient("192.168.1.6", 12345, message_buffer_factory);
    auto *client = new myrpc_LEDControl_Client("localhost", 12345);

    /* code */
    int32_t ret = 0;
    if (rpc_status::Success != client->open()) return -1;
    for (;;) {
        myrpc_Input req;
        myrpc_Output rsp;
        req.r = 1;
        req.g = 2;
        req.b = 3;
        rpc_status err = client->setColor(&req, &rsp);
        if (err != Success) {
            printf("qaq!\n");
            break;
        }
        sleep(1);
        printf("response: %" PRId32 "\n", rsp.success);
    }
    // printMatrix(result_matrix);
    /* other code like print result matrix */

    return 0;
}
// arch linux

//kde
