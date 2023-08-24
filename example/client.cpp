/*
 *  * Copyright (C), 2022-2023, Soochow University & OPPO Mobile Comm Corp., Ltd.
 *
 * File: client.cpp
 * Description: the client driver to test the RPC
 * Version: V1.0.0
 * Date: 2023/08/23
 * Author: Soochow University
 * Revision History:
 *   Version       Date          Author         Revision Description
 *  V1.0.0        2023/08/23    Soochow University       Create and initialize
 */
#include <stdio.h>
#include <unistd.h>

#include <iostream>
#include <thread>

#include "idl.pb.hpp"
// translate all comment above to English
int main(int argc, char **argv)
{
    std::atomic_bool stop(false);
    /* read input command */
    std::string cmd;
    /*create RPC Client*/
    auto *client = new myrpc_LEDControl_Client("localhost", 12345);
    /*open client*/
    if (rpc_status::Success != client->open()) {
        std::cout << "open failed" << std::endl;
        return 0;
    }
    while (std::cin >> cmd) {
        if (cmd == std::string("exit")) {
            std::cout << "bye" << std::endl;
            break;
        } else if (cmd == std::string("open")) {
            /*connect to ip:port(localhost:12345)*/
            if (rpc_status::Success != client->open()) {
                std::cout << "open failed" << std::endl;
                continue;
            }
        } else if (cmd == std::string("set")) {
            /*call RPC method*/
            std::cout << "r: ";
            std::string r;
            std::cin >> r;
            /*create rsp and req object*/
            myrpc_Input req;
            myrpc_Output rsp;
            /*set req arg*/
            req.color = const_cast<char *>(r.c_str());
            /*call RPC method and check if it succeed*/
            if (rpc_status::Success != client->setColor(&req, &rsp)) {
                std::cout << "rpc failed" << std::endl;
            } else {
                /*print rsp*/
                std::cout << "success, rsp = " << rsp.success << std::endl;
            }
            /*release rsp*/
            /*don't release req, for the space belongs to std::string r*/
            pb_release(myrpc_Output_fields, &rsp);
        } else if (cmd == std::string("close")) {
            /*close client, can be reopened*/
            std::cout << "closing" << std::endl;
            client->close();
        } else if (cmd == std::string("thread")) {
            /*how many thread?*/
            std::cout << "num: ";
            int num;
            std::cin >> num;
            /*create those thread*/
            for (int i = 0; i < num; ++i) {
                std::thread([i, num, &stop] {
                    /*client can't be used by multiple thread, each of them has to build a new client*/
                    auto *client = new myrpc_LEDControl_Client("localhost", 12345);

                    if (rpc_status::Success != client->open()) {
                        std::cout << "open failed" << std::endl;
                        return;
                    }
                    std::cout << "thread " << i + 1 << " started" << std::endl;

                    myrpc_Input req;
                    myrpc_Output rsp;
                    std::string name = "thread " + std::to_string(i);

                    req.color = const_cast<char *>(name.c_str());

                    int err = 0;
                    // sleep for i second in the beginning
                    std::this_thread::sleep_for(std::chrono::seconds(i));
                    // stop is the flag to stop all thread
                    while (!stop && err < 2) {
                        // call rpc method per num seconds
                        std::this_thread::sleep_for(std::chrono::seconds(num));
                        if (rpc_status::Success != client->setColor(&req, &rsp)) {
                            std::cout << "rpc failed" << std::endl;
                            ++err;
                        } else {
                            err = 0;
                            std::cout << "Thread " << (i + 1)
                                      << " : "
                                         "success, rsp = "
                                      << rsp.success << std::endl;
                        }
                        pb_release(myrpc_Output_fields, &rsp);
                    }
                    client->close();
                }).detach();
            }
        } else if (cmd == std::string("cancel")) {
            /*stop all thread*/
            stop = true;
        } else {
            std::cout << "unknown cmd" << std::endl;
        }
    }

    return 0;
}
// arch linux

// kde
