#include "server/simple_server.hpp"
#include "idl.pb.h"
#include <unistd.h>
#include <stdio.h>


int main()
{
    printf("qaq\n");
    /* Init eRPC server environment */

    /* MessageBufferFactory initialization */
    auto message_buffer_factory = new erpc::MessageBufferFactory();

    /* eRPC server side initialization */
    auto server = new erpc::SimpleServer("localhost", 12345, message_buffer_factory);
    /* add generated service into server, look into erpc_matrix_multiply_server.h */
    class myService:public myrpc_MatrixMultiplyService {
        void myrpctest(myrpc_InputTest *req, myrpc_OutputTest *rsp) {
            rsp->ret = req->num1 + req->num2;
        }
    };
    auto service = new myService();
    server->addService(service);
    server->open();

    /* run server */
    server->run();
    
    return 0;
}
