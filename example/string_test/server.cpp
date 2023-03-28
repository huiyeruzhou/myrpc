#include "qaq_server.h"
#include "simple_server.hpp"
#include <unistd.h>
#include <stdio.h>
/* implementation of function call */
void erpctest(const char *str) {
    printf("%s, called\n",str);
}

int main()
{
    printf("qaq\n");
    /* Init eRPC server environment */

    /* MessageBufferFactory initialization */
    auto message_buffer_factory = new erpc::MessageBufferFactory();

    /* eRPC server side initialization */
    auto server = new erpc::SimpleServer("localhost", 12345, message_buffer_factory);
    /* add generated service into server, look into erpc_matrix_multiply_server.h */
    auto service = new aaa_service();
    service->setName("");
    server->addService(service);
    server->open();

    /* run server */
    server->run();

    return 0;
}
