#ifndef _SERVICE_H_
#define _SERVICE_H_

#include "rpc_status.hpp"
#include "codec/meta.pb.h"
#include "codec/message_buffer.hpp"
#include "port/port.h"
namespace erpc
{
    class Service
    {
    public:
        /*!
         * @brief Constructor.
         *
         * This function initializes object attributes.
         */
        Service(const char * serviceId)
            : m_serviceId(serviceId)
            , m_next(NULL)
        {
        }

        /*!
         * @brief Service destructor
         */
        virtual ~Service(void) {}

        /*!
         * @brief Return service id number.
         *
         * @return Service id number.
         */
        const char *getServiceId(void) const { return m_serviceId; }

        /*!
         * @brief Return next service.
         *
         * @return Pointer to next service.
         */
        Service *getNext(void) { return m_next; }

        /*!
         * @brief Set next service.
         *
         * @param[in] next Pointer to next service.
         */
        void setNext(Service *next) { m_next = next; }

        virtual rpc_status handleInvocation(void *input, void *output) = 0;
        virtual void filledMsgDesc(const pb_msgdesc_t **input_desc, void **input_msg, const pb_msgdesc_t **output_desc,void **output) = 0;
        virtual void destroyMsg(void *input, void *output) = 0;


    protected:
        const char* m_serviceId; /*!< Service unique id. */
        Service *m_next;      /*!< Pointer to next service. */
        
    };

} // namespace erpc


#endif//_SERVICE_H_