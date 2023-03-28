#include "transport/tcp_transport.hpp"
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
using namespace erpc;

TCPWorker::~TCPWorker(void) {}

rpc_status TCPWorker::underlyingReceive(uint8_t *data, uint32_t size)
{
    ssize_t length;
    rpc_status status = Success;

    if(m_socket <= 0)
    {
        status = kErpcStatus_ConnectionFailure;
        return status;
    }

    // Loop until all requested data is received.
    while (size > 0U)
    {
        length = read(m_socket, data, size);

        // Length will be zero if the connection is closed.
        if (length > 0)
        {
            size -= length;
            data += length;
        }
        else
        {
            if (length == 0)
            {
                // close socket, not server
                close(false);
                status = kErpcStatus_ConnectionClosed;
            }
            else
            {
                status = kErpcStatus_ReceiveFailed;
                printf("transport:   unknown error from tcp, return value of read is %zu\n", length);
            }
            break;
        }
    }

    return status;
}

rpc_status TCPWorker::underlyingSend(const uint8_t *data, uint32_t size)
{
    rpc_status status = Success;
    ssize_t result;

    if (m_socket <= 0)
    {
        // we should not pretend to have a succesful Send or we create a deadlock
        status = kErpcStatus_ConnectionFailure;
    }
    else
    {
        // Loop until all data is sent.
        while (size > 0U)
        {
            result = write(m_socket, data, size);
            if (result >= 0)
            {
                size -= result;
                data += result;
            }
            else
            {
                if (result == EPIPE)
                {
                    // close socket, not server
                    close(false);
                    status = kErpcStatus_ConnectionClosed;
                }
                else
                {
                    status = kErpcStatus_SendFailed;
                }
                break;
            }
        }
    }

    return status;
}
