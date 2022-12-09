#ifndef _TCP_WORKER_H_
#define _TCP_WORKER_H_
#include "erpc_status.h"
#include "erpc_framed_transport.hpp"
#include <unistd.h>
////////////////////////////////////////////////////////////////////////////////
// Classes
////////////////////////////////////////////////////////////////////////////////
namespace erpc {
    /*!
     * @brief a worker send and receive data using lwip-tcp protocol
     *
     * This class defined a worker for m_socket, which is the accepted file descriptor of
     * m_port.
     *
     * @ingroup infra_transport
     */
    class TCPWorker : public FramedTransport {
    public:
        TCPWorker(int sockfd, int port) :
            FramedTransport(),
            m_socket(sockfd),
            m_port(port)
        {}
        ~TCPWorker() override;
        /*!
         * @brief Function to send message.
         *
         * use read()(\<unistd.h\>) to receive data until we got size bytes.
         *
         *
         * @param[out] data Bytearray buffer.
         * @param[in] size Excepting size of received data.
         *
         * @retval - kErpcStatus_Success when receive is successful
         * @retval - kErpcStatus_ConnectionClosed when read returns 0, which
         * implicates that client socket has been closed, or m_socket
         * is negative
         * @retval - kErpcStatus_ReceiveFailed when negative return value of read,
         * error message will be printed using strerror()
         */
        erpc_status_t underlyingReceive(uint8_t *data, uint32_t size) override;
        erpc_status_t underlyingSend(const uint8_t *data, uint32_t size) override;
        inline int getport() const {return m_port;};
        inline void close() const {::close(m_port);};
    private:
        int m_socket; //!<
        int m_port;
    };

}
#endif//_TCP_WORKER_H_