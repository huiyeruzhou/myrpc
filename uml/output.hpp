 
trixMultiplyService_service : public erpc::Service
{
public:
    MatrixMultiplyService_service() : Service(kMatrixMultiplyService_service_id)
    virtual erpc_status_t handleInvocation(uint32_t methodId, uint32_t sequence, erpc::Codec *codec, erpc::MessageBufferFactory *messageFactory);
private:
    erpc_status_t erpcMatrixMultiply_shim(erpc::Codec *codec, erpc::MessageBufferFactory *messageFactory, uint32_t sequence);
    erpc_status_t erpctest_shim(erpc::Codec *codec, erpc::MessageBufferFactory *messageFactory, uint32_t sequence);
};
