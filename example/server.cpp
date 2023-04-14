#include "server/simple_server.hpp"
#include "idl.pb.hpp"
#include <unistd.h>
#include <stdio.h>


int main()
{
    printf("qaq\n");
    

    auto server = new erpc::SimpleServer("localhost", 12345);
    /* add generated service into server, look into erpc_matrix_multiply_server.h */
    class myService:public myrpc_LEDControl_Service {
        void setColor(myrpc_Input *req, myrpc_Output *rsp) {
        }
    };
    auto service = new myService();
    server->addService(service);
    server->open();

    /* run server */
    server->run();
    
    return 0;
}
