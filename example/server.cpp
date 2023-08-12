#include <stdio.h>
#include <unistd.h>

#include <iostream>

#include "idl.pb.hpp"
#include "server/simple_server.hpp"

int main() {
  printf("qaq\n");

  auto server = new erpc::SimpleServer("localhost", 12345);
  class myService : public myrpc_LEDControl_Service {
    ~myService() override { printf("myService::~myService()\n"); }
    rpc_status setColor(myrpc_Input *req, myrpc_Output *rsp) override {
      rsp->success = true;
      printf("setColor: %s\n", req->color);
      return rpc_status::Success;
    }
  };
  auto service = new myService();
  server->addService(service);
  server->open();

  std::string cmd;
  while (std::cin >> cmd) {
    if (cmd == std::string("exit")) {
      std::cout << "bye" << std::endl;
      break;
    } else if (cmd == std::string("close")) {
      std::cout << "closing" << std::endl;
      server->close();
    } else if (cmd == std::string("open")) {
      std::cout << "opening" << std::endl;
      if (server == nullptr) {
        std::cout << "old server is deconstructed, new server" << std::endl;
        server = new erpc::SimpleServer("localhost", 12345);
        server->addService(new myService());
      }
      server->open();
    } else if (cmd == std::string("stop")) {
      std::cout << "stopping" << std::endl;
      server->stop();
      server = nullptr;
    } else {
      std::cout << "unknown cmd" << std::endl;
    }
  }

  return 0;
}
