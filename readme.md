## 简介
代码可以在linux下单独运行，或者作为esp-idf框架中的component运行。还可以作为Android项目中的原生方法运行(详见doc/Android.md)。
文件结构：
```bash
.
├── doc                     ## 文档
├── example                 ## 示例
├── infra                   ## 主体代码
├── mk                      ## makefile工具链
└── nanopb                  ## nanopb库(适配MyRPC)

```
## 环境配置（linux）

### 准备代码生成工具

需要安装protoc以及相应的库文件用于编译protobuf文件,
之后可以使用`nanopb/generator/protoc <yourFilename>.proto --nanopb_out=<outputDir>`来生成代码

### 编译项目库

在linux下，使用makefile进行编译。在根目录下运行
```bash
# cd myrpc
make
```
编译后的静态库位于`myrpc/Debug/Linux/erpc/lib/liberpc.a`，这个路径会因操作系统而改变。

### 为项目编写Makefile

参考`myrpc/example/Makefile`:

首先为nanopb编写Makefile，这会配置.proto文件的编译方法和Nanopb源代码文件
```Makefile
# Path to the nanopb root directory，可能需要修改
NANOPB_DIR := ../nanopb 
# Files for the nanopb core
NANOPB_CORE = $(NANOPB_DIR)/pb_encode.c $(NANOPB_DIR)/pb_decode.c $(NANOPB_DIR)/pb_common.c

...

# 注意，和原版Nanopb的扩展名不同
%.pb.cpp %.pb.hpp: %.proto %.options
	$(PROTOC) $(PROTOC_OPTS) --nanopb_out=. $<

%.pb.cpp %.pb.hpp: %.proto
	$(PROTOC) $(PROTOC_OPTS) --nanopb_out=. $<

# Compiler flags to enable all warnings & debug info
CFLAGS = -Wall -Werror -g -O0
CXXFLAGS += "-I$(NANOPB_DIR)" -I../include
```

然后为MyRPC编写Makefile，这会配置MyRPC库位置
```Makefile
# Path to erpc root directory,可能需要修改
ERPC_ROOT := $(abspath $(dir $(lastword $(MAKEFILE_LIST)))../)
ERPC_LIB=$(ERPC_ROOT)/Debug/Linux/erpc/lib/liberpc.a
CXXFLAGS += -g $(INCLUDE) -lpthread
```

最后为项目本身编写Makefile,配置idl文件的名称，client和server的编译方法：
```Makefile
# C source code files that are required
SRC  = server.cpp idl.pb.cpp ${NANOPB_CORE}            # The main program
CRC  = client.cpp idl.pb.cpp ${NANOPB_CORE}            # The main program

.PHONY: all clean
all: server client
clean:
	rm -f server client *.pb.cpp *.pb.hpp
# Build rule for the main program
server: $(SRC) $(ERPC_LIB)
	$(CXX) $(CXXFLAGS) -osimple $^
# Build rule for the main program
client: $(CRC) $(ERPC_LIB) 
	$(CXX) $(CXXFLAGS) -osimpleclient $^
```

## 项目配置（esp-idf框架）

### 准备代码生成工具(和Linux环境下相同)

### 构建component管理
在esp-idf框架中可以将本项目当做一个普通component来使用。详细情况参考[idf文档](https://docs.espressif.com/projects/esp-idf/zh_CN/latest/esp32/api-guides/build-system.html)。

具体步骤包括：
1. 将myrpc目录放置在idf项目的components目录下
```
.
├── CMakeLists.txt
├── README.md
├── build
├── components
|   ├── config_wifi
│   └── **myrpc**
├── main
│   ├── CMakeLists.txt
│   └── ...
├── partitions_example.csv
├── pytest_import_lib.py
├── sdkconfig
├── sdkconfig.defaults
└── sdkconfig.old
```
2. 在项目main目录的CMakeLists.txt中注册对myrpc的依赖
```cmake
idf_component_register(SRCS             ...#main源文件
                       INCLUDE_DIRS     ...#main包含目录
                        REQUIRES        ...#main的其他依赖
                       **myrpc**)       #把myrpc添加到REQUIRES中
```

3. 在main目录下写好proto文件，生成代码，并且编写对应的服务端或客户端代码。接下来，使用idf框架的编译工具链正常编译项目即可。

## 使用MyRPC编写代码

这部分的内容参考example。example中包含一个点灯示例，接收一个包含r、g、b三个整数的input，返回一个表示是否成功的布尔值success。

### 代码生成
编写一个.proto文件，使用标准proto2语法即可
```protobuf
syntax="proto2";

package myrpc; 

message Input{
    required int32 r = 1;
    required int32 g = 2;
    required int32 b = 3;
}

message Output{
    required bool success = 1;
}

service LEDControl
{
    rpc setColor(Input) returns (Output) {}
}

```

然后，在example目录下使用下面的命令（或者用之前Linux环境配置中的Makefile自动生成）
```bash
../nanopb/generator/protoc idl.proto --nanopb_out=.
```

### 使用生成的代码

代码生成包括一个.pb.cpp文件和一个.pb.hpp文件。文件的内容包括：
- 声明的结构体`<package>_<messageName>`
- 代理服务`<package>_<ServiceName>_Service`
- 代理客户端`<package>_<ServiceName>_Client`
  - 代理方法`<package>_<ServiceName>_Client::<methodName>`
  
### client端

这部分内容参考example/client.cpp

包含头文件

```c++
#include "idl.pb.hpp" //生成的代码
```

初始化client，需要传递服务器的ip地址，端口号。

```c++
 auto *client = new myrpc_LEDControl_Client("localhost", 12345);
```

调用client的open函数，尝试连接服务器
```c++
    if (rpc_status::Success != client->open()) return -1;
```

链接成功后使用erpc函数，需要创请求消息和响应消息对应的结构体，填写请求结构体的相应字段，
然后用两个结构体的地址调用代理方法。

接收到返回之后，校验是否成功，如果成功的话从响应信息中取出返回值，否则进行失败处理。
```c++
    for (;;) {
        myrpc_Input req;
        myrpc_Output rsp;
        req.r = 1;
        req.g = 2;
        req.b = 3;
        rpc_status err = client->setColor(&req, &rsp);
        if (err != Success) {
            printf("qaq!\n");
            break;
        }
        sleep(1);
        printf("response: %" PRId32 "\n", rsp.success);
    }
```

### server端
这部分内容参考example/server.cpp

包含头文件

```c++
#include "server/simple_server.hpp"         // 位于myrpc/infra/server中
#include "idl.pb.hpp"                       // 生成的文件
```

初始化server，需要传递ip地址，端口号。

```c++
auto server = new erpc::SimpleServer("localhost", 12345);
```

调用创建新的service继承代理服务，实现其服务方法。

```c++
    class myService:public myrpc_LEDControl_Service {
        rpc_status setColor(myrpc_Input *req, myrpc_Output *rsp) override {
            rsp->success = true;
            return rpc_status::Success;
        }
    };
```

将该服务注册到服务器中：
```c++
    auto service = new myService();
    server->addService(service);
```

启动服务器，从这一步开始可以建立网络连接
```c++
server->open();
```

