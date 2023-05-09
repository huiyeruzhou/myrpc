/* Automatically generated nanopb header */
/* Generated by nanopb-0.4.8-dev */

#ifndef PB_MYRPC_MATRIX_MULTIPLY_IDL_PB_HPP_INCLUDED
#define PB_MYRPC_MATRIX_MULTIPLY_IDL_PB_HPP_INCLUDED
#include <pb.h>
#include <server/service.hpp>
#include <client/rpc_client.hpp>
#include <rpc_status.hpp>
#include <pb_encode.h>
#include <pb_decode.h>
#include <functional>

#if PB_PROTO_HEADER_VERSION != 40
#error Regenerate this file with the current version of nanopb generator.
#endif

/* Enum definitions */
typedef enum _myrpc_matrix_multiply_e {
    myrpc_matrix_multiply_e_e1 = 1,
    myrpc_matrix_multiply_e_e2 = 2
} myrpc_matrix_multiply_e;

/* Struct definitions */
typedef struct _myrpc_matrix_multiply_proto_Nested {
    int32_t num1;
    int32_t num2;
} myrpc_matrix_multiply_proto_Nested;

typedef struct _myrpc_matrix_multiply_proto {
    int32_t num1;
    myrpc_matrix_multiply_proto_Nested nested;
    char *qaq;
    pb_size_t waw_count;
    int32_t *waw;
} myrpc_matrix_multiply_proto;

typedef struct _myrpc_matrix_multiply_InputTest {
    int32_t num1;
    int32_t num2;
} myrpc_matrix_multiply_InputTest;

typedef struct _myrpc_matrix_multiply_OutputTest {
    int32_t ret;
} myrpc_matrix_multiply_OutputTest;


#ifdef __cplusplus
extern "C" {
#endif

/* Helper constants for enums */
#define _myrpc_matrix_multiply_e_MIN myrpc_matrix_multiply_e_e1
#define _myrpc_matrix_multiply_e_MAX myrpc_matrix_multiply_e_e2
#define _myrpc_matrix_multiply_e_ARRAYSIZE ((myrpc_matrix_multiply_e)(myrpc_matrix_multiply_e_e2+1))






/* Initializer values for message structs */
#define myrpc_matrix_multiply_proto_init_default {0, myrpc_matrix_multiply_proto_Nested_init_default, NULL, 0, NULL}
#define myrpc_matrix_multiply_proto_Nested_init_default {0, 0}
#define myrpc_matrix_multiply_InputTest_init_default {0, 0}
#define myrpc_matrix_multiply_OutputTest_init_default {0}
#define myrpc_matrix_multiply_proto_init_zero    {0, myrpc_matrix_multiply_proto_Nested_init_zero, NULL, 0, NULL}
#define myrpc_matrix_multiply_proto_Nested_init_zero {0, 0}
#define myrpc_matrix_multiply_InputTest_init_zero {0, 0}
#define myrpc_matrix_multiply_OutputTest_init_zero {0}

/* Field tags (for use in manual encoding/decoding) */
#define myrpc_matrix_multiply_proto_Nested_num1_tag 1
#define myrpc_matrix_multiply_proto_Nested_num2_tag 2
#define myrpc_matrix_multiply_proto_num1_tag     1
#define myrpc_matrix_multiply_proto_nested_tag   2
#define myrpc_matrix_multiply_proto_qaq_tag      3
#define myrpc_matrix_multiply_proto_waw_tag      4
#define myrpc_matrix_multiply_InputTest_num1_tag 1
#define myrpc_matrix_multiply_InputTest_num2_tag 2
#define myrpc_matrix_multiply_OutputTest_ret_tag 1

/* Struct field encoding specification for nanopb */
#define myrpc_matrix_multiply_proto_FIELDLIST(X, a) \
X(a, STATIC,   REQUIRED, INT32,    num1,              1) \
X(a, STATIC,   REQUIRED, MESSAGE,  nested,            2) \
X(a, POINTER,  REQUIRED, STRING,   qaq,               3) \
X(a, POINTER,  REPEATED, INT32,    waw,               4)
#define myrpc_matrix_multiply_proto_CALLBACK NULL
#define myrpc_matrix_multiply_proto_DEFAULT NULL
#define myrpc_matrix_multiply_proto_nested_MSGTYPE myrpc_matrix_multiply_proto_Nested

#define myrpc_matrix_multiply_proto_Nested_FIELDLIST(X, a) \
X(a, STATIC,   REQUIRED, INT32,    num1,              1) \
X(a, STATIC,   REQUIRED, INT32,    num2,              2)
#define myrpc_matrix_multiply_proto_Nested_CALLBACK NULL
#define myrpc_matrix_multiply_proto_Nested_DEFAULT NULL

#define myrpc_matrix_multiply_InputTest_FIELDLIST(X, a) \
X(a, STATIC,   REQUIRED, INT32,    num1,              1) \
X(a, STATIC,   REQUIRED, INT32,    num2,              2)
#define myrpc_matrix_multiply_InputTest_CALLBACK NULL
#define myrpc_matrix_multiply_InputTest_DEFAULT NULL

#define myrpc_matrix_multiply_OutputTest_FIELDLIST(X, a) \
X(a, STATIC,   REQUIRED, INT32,    ret,               1)
#define myrpc_matrix_multiply_OutputTest_CALLBACK NULL
#define myrpc_matrix_multiply_OutputTest_DEFAULT NULL

extern const pb_msgdesc_t myrpc_matrix_multiply_proto_msg;
extern const pb_msgdesc_t myrpc_matrix_multiply_proto_Nested_msg;
extern const pb_msgdesc_t myrpc_matrix_multiply_InputTest_msg;
extern const pb_msgdesc_t myrpc_matrix_multiply_OutputTest_msg;

/* Defines for backwards compatibility with code written before nanopb-0.4.0 */
#define myrpc_matrix_multiply_proto_fields &myrpc_matrix_multiply_proto_msg
#define myrpc_matrix_multiply_proto_Nested_fields &myrpc_matrix_multiply_proto_Nested_msg
#define myrpc_matrix_multiply_InputTest_fields &myrpc_matrix_multiply_InputTest_msg
#define myrpc_matrix_multiply_OutputTest_fields &myrpc_matrix_multiply_OutputTest_msg

/* Maximum encoded size of messages (where known) */
/* myrpc_matrix_multiply_proto_size depends on runtime parameters */
#define myrpc_matrix_multiply_InputTest_size     22
#define myrpc_matrix_multiply_OutputTest_size    11
#define myrpc_matrix_multiply_proto_Nested_size  22

/* Service Definations */
class myrpc_matrix_multiply_MatrixMultiplyService : public erpc::Service {
public:
    myrpc_matrix_multiply_MatrixMultiplyService() {}
    virtual ~myrpc_matrix_multiply_MatrixMultiplyService() {}
    rpc_status myrpctest(myrpc_matrix_multiply_InputTest *request, myrpc_matrix_multiply_OutputTest *response);
};

/* Client Defination */
class myrpc_matrix_multiply_MatrixMultiplyService : public erpc::Client {
public:
    myrpc_matrix_multiply_MatrixMultiplyService(const char *host, uint16_t port): erpc::Client(host, port) {}
    virtual ~myrpc_matrix_multiply_MatrixMultiplyService() {}
rpc_status myrpc_matrix_multiply_MatrixMultiplyService::myrpctest(myrpc_matrix_multiply_InputTest *req, myrpc_matrix_multiply_OutputTest *rsp) {
    LOGW(myrpc_matrix_multiply_MatrixMultiplyService_method_names[0], "Service Unimplemented!");
    return rpc_status::UnimplmentedService;
}
};
#ifdef __cplusplus
} /* extern "C" */
#endif

#endif
