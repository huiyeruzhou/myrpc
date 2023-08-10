#include "server/server_base.hpp"

erpc::Server::~Server() {
  // LOGE("memory", "server deconstruct methods=%ld", methods.use_count());
}

void erpc::Server::addService(Service *service) {
  for (auto method : service->methods) {
    this->methods->emplace_back(std::shared_ptr<MethodBase>(method));
  }
}

void erpc::Server::removeService(Service *service) {
  this->methods->erase(
      std::remove_if(this->methods->begin(), this->methods->end(),
                     [&](std::shared_ptr<MethodBase> &method) {
                       return std::find(service->methods.begin(),
                                        service->methods.end(),
                                        method.get()) != service->methods.end();
                     }),
      this->methods->end());
}
