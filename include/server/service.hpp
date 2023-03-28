#ifndef _SERVICE_H_
#define _SERVICE_H_

#include "codec/codec_base.hpp"
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
        Service(uint32_t serviceId)
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
        uint32_t getServiceId(void) const { return m_serviceId; }

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

        void setName(const char *name) { m_name = name; }

        /*!
         * @brief This function call function implementation of current service.
         *
         * @param[in] methodId Id number of function, which is requested.
         * @param[in] sequence Sequence number. To be sure that reply from server belongs to client request.
         * @param[in] codec For reading and writing data.
         * @param[in] messageFactory Used for setting output buffer.
         *
         * @return Based on handleInvocation implementation.
         */
        virtual rpc_status handleInvocation(uint32_t methodId, uint32_t sequence, Codec *codec,
                                            MessageBufferFactory *messageFactory) = 0;

        const char *m_name;

    protected:
        uint32_t m_serviceId; /*!< Service unique id. */
        Service *m_next;      /*!< Pointer to next service. */
        
    };

} // namespace erpc


#endif//_SERVICE_H_