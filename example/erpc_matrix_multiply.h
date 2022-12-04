/*
 * You can write copyrights rules here. These rules will be copied into the outputs.
 */

/*
 * Generated by erpcgen 1.9.1 on Mon Dec  5 03:31:02 2022.
 *
 * AUTOGENERATED - DO NOT EDIT
 */


#if !defined(_erpc_matrix_multiply_h_)
#define _erpc_matrix_multiply_h_

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "erpc_version.h"

#if 10901 != ERPC_VERSION_NUMBER
#error "The generated shim code version is different to the rest of eRPC code."
#endif

#if !defined(ERPC_TYPE_DEFINITIONS)
#define ERPC_TYPE_DEFINITIONS

// Aliases data types declarations
/*! This is the matrix array type. The dimension has to be the same as the
    matrix size const. Do not forget to re-generate the eRPC code once the
    matrix size is changed in the eRPC file */
typedef int32_t Matrix[2][2];

// Constant variable declarations
/*! This const defines the matrix size. The value has to be the same as the
    Matrix array dimension. Do not forget to re-generate the eRPC code once the
    matrix size is changed in the eRPC file */
extern const int32_t matrix_size;

#endif // ERPC_TYPE_DEFINITIONS

/*! @brief MatrixMultiplyService identifiers */
enum _MatrixMultiplyService_ids
{
    kMatrixMultiplyService_service_id = 1,
    kMatrixMultiplyService_erpcMatrixMultiply_id = 1,
    kMatrixMultiplyService_erpctest_id = 2,
};

#if defined(__cplusplus)
extern "C" {
#endif

//! @name MatrixMultiplyService
//@{
void erpcMatrixMultiply(Matrix matrix1, Matrix matrix2, Matrix result_matrix);

void erpctest(int32_t num1, int32_t num2, int32_t * ret);
//@}

#if defined(__cplusplus)
}
#endif

#endif // _erpc_matrix_multiply_h_
