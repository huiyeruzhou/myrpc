#include "server/server_base.hpp"

void erpc::Server::addService(Service *service) {
  this->methods.insert(this->methods.end(), service->methods.begin(),
                       service->methods.end());
}

void erpc::Server::removeService(Service *service) {
  this->methods.erase(
      std::remove_if(this->methods.begin(), this->methods.end(),
                     [&](MethodBase *method) {
                       return std::find(service->methods.begin(),
                                        service->methods.end(),
                                        method) != service->methods.end();
                     }),
      this->methods.end());
}
