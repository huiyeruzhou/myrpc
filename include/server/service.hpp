#ifndef _SERVICE_H_
#define _SERVICE_H_

#include "rpc_status.hpp"
#include "codec/meta.pb.h"
#include "codec/message_buffer.hpp"
#include "port/port.h"
#include <functional>
#include <algorithm>
#include <vector>
namespace erpc
{

    class MethodBase {

    public:
        MethodBase(const char *serviceId, const pb_msgdesc_t *input_desc, const pb_msgdesc_t *output_desc)
            : m_path(serviceId)
            , m_input_desc(input_desc)
            , m_output_desc(output_desc) {
        }

        virtual ~MethodBase(void) {}
        const char *getPath(void) const { return m_path; }
        virtual rpc_status handleInvocation(void *input, void *output) = 0;
        virtual void filledMsgDesc(const pb_msgdesc_t **input_desc, void **input_msg, const pb_msgdesc_t **output_desc, void **output) = 0;
        virtual void destroyMsg(void *input, void *output) = 0;
    protected:
        const char *m_path; /*!< Service unique id. */
        const pb_msgdesc_t *m_input_desc;
        const pb_msgdesc_t *m_output_desc;
    };

    class Service{
    public:
        Service(){}
        virtual ~Service(void){}
        void addMethod(MethodBase *method){methods.emplace_back(method);}
        std::vector<erpc::MethodBase*> methods;
    };

    // class MethodHandlerBase
    // {
    // public:
    //     virtual ~MethodHandlerBase() {}
    //     virtual rpc_status invoke(erpc::MethodBase *service, void *input, void *output) = 0;
    // };

    // template <class ServiceType, class InputType, class OutputType>
    // class MethodHandler: public MethodHandlerBase
    // {
    // public:
    //     MethodHandler(std::function<rpc_status(ServiceType *, InputType *, OutputType *)> func)
    //         : func(func) {
    //     }
    //     virtual ~MethodHandler() {
    //     }
    //     virtual rpc_status invoke(erpc::MethodBase *service, void *input, void *output) override {
    //         return func(reinterpret_cast<ServiceType*>(service), (InputType*) input, (OutputType*)output);
    //     }
    // private:
    //     std::function<rpc_status(ServiceType *, InputType *, OutputType *)> func;
    // };
    
    template <class InputType, class OutputType>
    class Method: public MethodBase
    {
    public:
        /*!
         * @brief Constructor.
         *
         * This function initializes object attributes.
         */
        Method(const char *serviceId, const pb_msgdesc_t *input_desc, const pb_msgdesc_t *output_desc,
               std::function<rpc_status(Service *, InputType *, OutputType *)> func, Service* service)
            : MethodBase(serviceId, input_desc, output_desc),
              func(func),
              service(service)
        {
        }
        virtual ~Method() {
        }

        virtual rpc_status handleInvocation(void *input, void *output){
            return func(reinterpret_cast<erpc::Service*>(service), (InputType*)input, (OutputType*)output);
        }
        void filledMsgDesc(const pb_msgdesc_t **input_desc, void **input_msg, const pb_msgdesc_t **output_desc, void **output) override {
            *input_desc = m_input_desc;
            *input_msg = new InputType();
            *output_desc = m_output_desc;
            *output = new OutputType();
        };
        void destroyMsg(void *input, void *output) override { if (input) delete (InputType *) input; if (output) delete (OutputType *) output; }
    private:
        std::function<rpc_status(Service *, InputType *, OutputType *)> func;
        Service* service;
    };

} // namespace erpc


#endif//_SERVICE_H_