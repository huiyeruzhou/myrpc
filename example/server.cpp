#include "erpc_matrix_multiply_server.h"
#include "simple_server.hpp"
#include <unistd.h>
#include <stdio.h>
/* implementation of function call */
void erpcMatrixMultiply(Matrix matrix1, Matrix matrix2, Matrix result_matrix)
{
    /* code for multiplication of matrices */
    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 2; j++) {
            result_matrix[i][j] = 0;
            for (int k = 0; k < 2; k++)
                result_matrix[i][j] += matrix1[i][k] * matrix2[k][j];
        }
    }
    sleep(3);
}

void erpctest(int32_t num1, int32_t num2, int32_t *ret) {
    printf("called\n");
    *ret = num1 + num2;
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
    auto service = new MatrixMultiplyService_service();
    service->setName("Test Service");
    server->addService(service);
    server->open();

    /* run server */
    server->run();
    
    return 0;
}
