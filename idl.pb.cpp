/* Automatically generated nanopb constant definitions */
/* Generated by nanopb-0.4.8-dev */

#include "idl.pb.hpp"
#if PB_PROTO_HEADER_VERSION != 40
#error Regenerate this file with the current version of nanopb generator.
#endif

PB_BIND(myrpc_matrix_multiply_proto, myrpc_matrix_multiply_proto, AUTO)


PB_BIND(myrpc_matrix_multiply_proto_Nested, myrpc_matrix_multiply_proto_Nested, AUTO)


PB_BIND(myrpc_matrix_multiply_InputTest, myrpc_matrix_multiply_InputTest, AUTO)


PB_BIND(myrpc_matrix_multiply_OutputTest, myrpc_matrix_multiply_OutputTest, AUTO)




/* Name table */
static const char* myrpc_matrix_multiply_MatrixMultiplyService_method_names[] = {
    "/myrpc_matrix_multiply.MatrixMultiplyService/myrpctest",
};
/* Method Registration */
myrpc_matrix_multiply_MatrixMultiplyService_Service::myrpc_matrix_multiply_MatrixMultiplyService_Service() {
       addMethod(new erpc::Method<myrpc_matrix_multiply_InputTest, myrpc_matrix_multiply_OutputTest>(
               myrpc_matrix_multiply_MatrixMultiplyService_method_names[0], myrpc_matrix_multiply_InputTest_fields, myrpc_matrix_multiply_OutputTest_fields,
               [](Service *s, myrpc_matrix_multiply_InputTest *i, myrpc_matrix_multiply_OutputTest *o)->rpc_status {return reinterpret_cast<myrpc_matrix_multiply_MatrixMultiplyService_Service*>(s)->myrpctest(i, o);},
               this));
}
/* Server stub */
rpc_status myrpc_matrix_multiply_MatrixMultiplyService_Service::myrpctest(myrpc_matrix_multiply_InputTest *req, myrpc_matrix_multiply_OutputTest *rsp) {
    LOGW(myrpc_matrix_multiply_MatrixMultiplyService_method_names[0], "Service Unimplemented!");
    return rpc_status::UnimplmentedService;
}


/* Client stub */
rpc_status myrpc_matrix_multiply_MatrixMultiplyService_Client::myrpctest(myrpc_matrix_multiply_InputTest *req, myrpc_matrix_multiply_OutputTest *rsp) {
    return performRequest(const_cast<char *>(myrpc_matrix_multiply_MatrixMultiplyService_method_names[0]), myrpc_matrix_multiply_InputTest_fields, (void *) req, myrpc_matrix_multiply_OutputTest_fields, (void *) rsp);
}

