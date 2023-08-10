#ifndef SERVER_SERVICE_HPP_
#define SERVER_SERVICE_HPP_

#include <algorithm>
#include <functional>
#include <memory>
#include <vector>

#include "codec/message_buffer.hpp"
#include "codec/meta.pb.h"
#include "port/port.h"
#include "rpc_status.hpp"
namespace erpc {

class MethodBase {
 public:
  MethodBase(const char *serviceId, const pb_msgdesc_t *input_desc,
             const pb_msgdesc_t *output_desc)
      : m_path(serviceId),
        m_input_desc(input_desc),
        m_output_desc(output_desc) {}

  virtual ~MethodBase(void) { LOGE("Method", "MethodBase::~MethodBase"); }
  const char *getPath(void) const { return m_path; }
  virtual rpc_status handleInvocation(void *input, void *output) = 0;
  virtual void filledMsgDesc(const pb_msgdesc_t **input_desc, void **input_msg,
                             const pb_msgdesc_t **output_desc,
                             void **output) = 0;
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
  Method(const char *serviceId, const pb_msgdesc_t *input_desc,
         const pb_msgdesc_t *output_desc,
         std::function<rpc_status(Service *, InputType *, OutputType *)> func,
         std::shared_ptr<Service> service)
      : MethodBase(serviceId, input_desc, output_desc),
        func(func),
        service(service) {}
  virtual ~Method() {}

  rpc_status handleInvocation(void *input, void *output) override {
    return func(service.get(), static_cast<InputType *>(input),
                static_cast<OutputType *>(output));
  }
  void filledMsgDesc(const pb_msgdesc_t **input_desc, void **input_msg,
                     const pb_msgdesc_t **output_desc, void **output) override {
    *input_desc = m_input_desc;
    *input_msg = new InputType();
    *output_desc = m_output_desc;
    *output = new OutputType();
  };
  void destroyMsg(void *input, void *output) override {
    if (input) {
      delete static_cast<InputType *>(input);
    }
    if (output) {
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
