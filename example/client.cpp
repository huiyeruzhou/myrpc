#include <stdio.h>
#include <unistd.h>

#include <iostream>

#include "idl.pb.hpp"

int main(int argc, char **argv) {
  /* eRPC client side initialization */

  /* code */
  int32_t ret = 0;
  std::string cmd;
  auto *client = new myrpc_LEDControl_Client("localhost", 12345);
  while (std::cin >> cmd) {
    if (cmd == std::string("exit")) {
      std::cout << "bye" << std::endl;
      break;
    } else if (cmd == std::string("open")) {
      if (rpc_status::Success != client->open()) {
        std::cout << "open failed" << std::endl;
        continue;
      }
    } else if (cmd == std::string("set")) {
      std::cout << "r: ";
      std::string r;
      std::cin >> r;
      myrpc_Input req;
      myrpc_Output rsp;
      req.color = const_cast<char *>(r.c_str());
      if (rpc_status::Success != client->setColor(&req, &rsp)) {
        std::cout << "rpc failed" << std::endl;
      } else {
        std::cout << "success, rsp = " << rsp.success << std::endl;
      }
      printf("setColor: %s\n", req.color);
      pb_release(myrpc_Input_fields , & req);
      printf("setColor: %s\n", req.color);
    } else if (cmd == std::string("close")) {
      std::cout << "closing" << std::endl;
      client->close();
    } else {
      std::cout << "unknown cmd" << std::endl;
    }
  }

  return 0;
}
// arch linux

// kde
