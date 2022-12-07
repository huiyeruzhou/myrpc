#include "erpc_matrix_multiply_server.h"
#include "erpc_simple_server.hpp"
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
    *ret = num1 + 10;
    usleep(num2 * 1000000);
}

int main()
{
    printf("qaq\n");
    /* Init eRPC server environment */
    /* UART transport layer initialization */

    /* MessageBufferFactory initialization */
    auto message_buffer_factory = new erpc::MessageBufferFactory();

    /* eRPC server side initialization */
    auto server = new erpc::SimpleServer("localhost", 12345, message_buffer_factory);
    /* connect generated service into server, look into erpc_matrix_multiply_server.h */
    erpc_service_t service = create_MatrixMultiplyService_service();
    erpc::Service *ser = (erpc::Service *)(service);
    ser->setName("Test Service");
    server->addService(ser);
    server->open();

    /* run server */
    server->run();
    
    return 0;
}
