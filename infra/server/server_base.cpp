#include "server/server_base.hpp"

using namespace erpc;

////////////////////////////////////////////////////////////////////////////////
// Code
////////////////////////////////////////////////////////////////////////////////

#if ERPC_NESTED_CALLS_DETECTION
extern bool nestingDetection;
bool nestingDetection = false;
#endif

void Server::addService(Service *service)
{
    this->methods.insert(this->methods.end(), service->methods.begin(), service->methods.end());
}

void Server::removeService(Service *service)
{
    this->methods.erase(std::remove_if(this->methods.begin(), this->methods.end(), [&](MethodBase *method) {
        return std::find(service->methods.begin(), service->methods.end(), method) != service->methods.end();
    }), this->methods.end());
}



