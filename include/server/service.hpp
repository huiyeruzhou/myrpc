/*
 * Copyright (C), 2022-2023, Soochow University & OPPO Mobile Comm Corp., Ltd.
 *
 * File: server_service.hpp
 * Description: This file defines the Service class. MethodBase class. and the Method class.
 *              Service class is used to hold the methods(and some data used by methods) of the service.
 *              MethodBase class is the base class of the method.
 *              Method class is template class, which is used to define the method.
 *              All Method in Serice share the Service object and will destroy it when all method is destroyed.
 *              A Method is shared by the Server and the ServerWorker thread which is processing a request of the method.
 * Version: V1.0.0
 * Date: 2023/08/23
 * Author: Soochow University
 * Revision History:
 *   Version       Date          Author         Revision Description
 *  V1.0.0        2023/08/23    Soochow University       Create and initialize
 */
#ifndef SERVER_SERVICE_HPP_
#define SERVER_SERVICE_HPP_

#include <algorithm>
#include <functional>
#include <memory>
#include <vector>

#include "codec/message_buffer.hpp"
#include "codec/meta.pb.hpp"
#include "nanopb/pb_decode.h"
#include "port/port.h"
#include "rpc_status.hpp"
namespace erpc {

class MethodBase {
   public:
    MethodBase(const char *serviceId, const pb_msgdesc_t *input_desc, const pb_msgdesc_t *output_desc)
        : m_path(serviceId), m_input_desc(input_desc), m_output_desc(output_desc)
    {
    }

    virtual ~MethodBase(void) { LOGE(m_path, "deonstructed"); }
    const char *getPath(void) const { return m_path; }
    virtual rpc_status handleInvocation(void *input, void *output) = 0;
    virtual void filledMsgDesc(const pb_msgdesc_t **input_desc, void **input_msg, const pb_msgdesc_t **output_desc, void **output) = 0;
    virtual void destroyMsg(void *input, void *output) = 0;

   protected:
    const char *m_path; /*!< Service unique id. */
    const pb_msgdesc_t *m_input_desc;
    const pb_msgdesc_t *m_output_desc;
};

class Service {
   public:
    Service() {}
    virtual ~Service(void) {}
    void addMethod(MethodBase *method) { methods.emplace_back(method); }
    std::vector<erpc::MethodBase *> methods;
};

template <class InputType, class OutputType>
class Method : public MethodBase {
   public:
    /*!
     * @brief Constructor.
     *
     * This function initializes object attributes.
     */
    Method(const char *serviceId, const pb_msgdesc_t *input_desc, const pb_msgdesc_t *output_desc,
           std::function<rpc_status(Service *, InputType *, OutputType *)> func, std::shared_ptr<Service> service)
        : MethodBase(serviceId, input_desc, output_desc), func(func), service(service)
    {
    }
    virtual ~Method() {}

    rpc_status handleInvocation(void *input, void *output) override
    {
        return func(service.get(), static_cast<InputType *>(input), static_cast<OutputType *>(output));
    }
    void filledMsgDesc(const pb_msgdesc_t **input_desc, void **input_msg, const pb_msgdesc_t **output_desc, void **output) override
    {
        *input_desc = m_input_desc;
        *input_msg = new InputType();
        *output_desc = m_output_desc;
        *output = new OutputType();
    };
    void destroyMsg(void *input, void *output) override
    {
        if (input) {
            pb_release(m_input_desc, input);
            delete static_cast<InputType *>(input);
        }
        if (output) {
            pb_release(m_output_desc, output);
            delete static_cast<OutputType *>(output);
        }
    }

   private:
    std::function<rpc_status(Service *, InputType *, OutputType *)> func;
    std::shared_ptr<Service> service;
};
typedef std::vector<std::shared_ptr<erpc::MethodBase>> MethodVector;
}  // namespace erpc

#endif  // SERVER_SERVICE_HPP_
