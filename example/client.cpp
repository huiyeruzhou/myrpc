#include <stdio.h>
#include <unistd.h>

#include <iostream>
#include <thread>

#include "idl.pb.hpp"

int main(int argc, char **argv) {
  std::atomic_bool stop(false);
  /* 读取输入命令 */
  std::string cmd;
  /*创建RPC客户端*/
  auto *client = new myrpc_LEDControl_Client("localhost", 12345);
  /*启动RPC客户端*/
  if (rpc_status::Success != client->open()) {
    std::cout << "open failed" << std::endl;
    return 0;
  }
  while (std::cin >> cmd) {
    if (cmd == std::string("exit")) {
      /*退出*/
      std::cout << "bye" << std::endl;
      break;
    } else if (cmd == std::string("open")) {
      /*启动客户端:根据ip/port建立到服务器的连接*/
      if (rpc_status::Success != client->open()) {
        std::cout << "open failed" << std::endl;
        continue;
      }
    } else if (cmd == std::string("set")) {
      /*调用RPC方法, 读取一个字符串作为请求参数*/
      std::cout << "r: ";
      std::string r;
      std::cin >> r;
      /*创建RPC请求和响应对象*/
      myrpc_Input req;
      myrpc_Output rsp;
      /*设置请求参数*/
      req.color = const_cast<char *>(r.c_str());
      /*调用RPC方法并检查是否成功*/
      if (rpc_status::Success != client->setColor(&req, &rsp)) {
        std::cout << "rpc failed" << std::endl;
      } else {
        /*打印响应结果*/
        std::cout << "success, rsp = " << rsp.success << std::endl;
      }
      /*释放RPC响应结构体中分配的空间*/
      /*不要释放请求结构体中的空间, 因为那是由std::string管理的*/
      pb_release(myrpc_Output_fields, &rsp);
    } else if (cmd == std::string("close")) {
      /*关闭客户端, 之后可重新打开*/
      std::cout << "closing" << std::endl;
      client->close();
    } else if (cmd == std::string("thread")) {
      /*输入需要多少个*/
      std::cout << "num: ";
      int num;
      std::cin >> num;
      /*创建这些线程, 不断以自身序号调用RPC直到收到停止信号*/
      for (int i = 0; i < num; ++i) {
        std::thread([i, num, &stop] {
          /*客户端不能被多个线程并发使用, 每个线程都要有自己的客户端*/
          auto *client = new myrpc_LEDControl_Client("localhost", 12345);
          /*启动服务器*/
          if (rpc_status::Success != client->open()) {
            std::cout << "open failed" << std::endl;
            return;
          }
          std::cout << "thread " << i + 1 << " started" << std::endl;
          /*创建RPC请求和响应对象*/
          myrpc_Input req;
          myrpc_Output rsp;
          std::string name = "thread " + std::to_string(i);
          /*设置请求参数*/
          req.color = const_cast<char *>(name.c_str());
          /*调用RPC方法, 直到收到停止信号或错误数超标*/
          int err = 0;
          std::this_thread::sleep_for(std::chrono::seconds(i));
          while (!stop && err < 2) {
            std::this_thread::sleep_for(std::chrono::seconds(num));
            if (rpc_status::Success != client->setColor(&req, &rsp)) {
              std::cout << "rpc failed" << std::endl;
              ++err;
            } else {
              /*打印响应结果*/
              err = 0;
              std::cout << "Thread " << (i + 1)
                        << " : "
                           "success, rsp = "
                        << rsp.success << std::endl;
            }
            /*释放RPC响应结构体中分配的空间*/
            /*不要释放请求结构体中的空间, 因为那是由std::string管理的*/
            pb_release(myrpc_Output_fields, &rsp);
          }
          /*关闭客户端*/
          client->close();
        }).detach();
      }
    } else if (cmd == std::string("cancel")) {
      /*停止所有线程*/
      stop = true;
    } else {
      std::cout << "unknown cmd" << std::endl;
    }
  }

  return 0;
}
// arch linux

// kde
