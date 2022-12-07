#include "erpc_matrix_multiply.h"
#include "erpc_client_setup.h"

#include <stdio.h>

void printMatrix(const Matrix m) {
    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 2; j++)
            printf("%d\t", m[i][j]);
        putchar('\n');
    }
    putchar('\n');
}

int main(int argc,char** argv)
{
    /* Matrices definitions */
    Matrix matrix1, matrix2, result_matrix = {{0}};
    /* MessageBufferFactory initialization */
    erpc_mbf_t message_buffer_factory = erpc_mbf_dynamic_init();

    /* eRPC client side initialization */
    erpc_client_t client = erpc_client_init("localhost", 12345, message_buffer_factory);
    
    
    int num1 = argv[1][0] - '0';
    int num2 = argv[2][0] - '0';
    /* call eRPC functions */

        /* other code like init matrix1 and matrix2 values */
        // matrix1[0][0] = argv[1][0];
        // // matrix1[0][0] = i;
        // matrix1[0][1] = 3;
        // matrix1[1][0] = 5;
        // matrix1[1][1] = 7;
        // printMatrix(matrix1);
        // matrix2[0][0] = 11;
        // matrix2[0][1] = 13;
        // matrix2[1][0] = 17;
        // matrix2[1][1] = 19;
        // printMatrix(matrix2);
    printf("perform request %d\n", num1);
    

        /* code */
        // erpcMatrixMultiply(matrix1, matrix2, result_matrix);
    int ret;
    ret = 0;
    erpctest(num1, num2, &ret);
    printf("response: %d\n", ret);
    // printMatrix(result_matrix);
    /* other code like print result matrix */
    
    return 0;
}
// arch linux

//kde
