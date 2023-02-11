## 简介
代码可以在linux下单独运行，或者作为esp-idf框架中的component运行。
文件结构：
```bash
.
├── Debug                   ## makefile构建目录
├── build                   ## cmake构建目录
├── doc                     ## 文档
├── erpcgen                 ## 代码生成工具
├── example                 ## 示例
├── infra                   ## 主体代码
└── mk                      ## makefile工具链
```
## 编译（linux）

### 编译代码生成工具
在erpcgen目录下运行:
```bash
# cd myrpc/erpcgen
make
```
生成的工具位于`myrpc/Debug/Linux/erpcgen/erpcgen`

### 编译项目库
在linux下，使用makefile进行编译。在根目录下运行
```bash
# cd myrpc
make
```
编译后的静态库位于`myrpc/Debug/Linux/erpc/lib/liberpc.a`，这个路径会因操作系统而改变。

### 使用项目头文件和库
要使用myrpc，需要在编译时加入编译选项来指示头文件和库文件的位置,参考`myrpc/example/Makefile`中的
```make
ERPC_LIB=$(ERPC_ROOT)/Debug/Linux/erpc/lib/liberpc.a
INCLUDES += $(ERPC_C_ROOT)/infra \
			$(ERPC_C_ROOT)/infra/codec \
			$(ERPC_C_ROOT)/infra/client \
			$(ERPC_C_ROOT)/infra/server \
			$(ERPC_C_ROOT)/infra/transport \
			$(ERPC_C_ROOT)/infra/port \
			$(ERPC_C_ROOT)/infra/port/threading\
			$(ERPC_C_ROOT)/infra/port/malloc_free\
			$(ERPC_C_ROOT)/infra/port/log 
INCLUDE := $(foreach includes, $(INCLUDES), -I $(includes))
```



## 编译（esp-idf框架）
### 编译代码生成工具
在erpcgen目录下运行:
```bash
# cd myrpc/erpcgen
make
```
生成的工具位于`myrpc/Debug/Linux/erpcgen/erpcgen`

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
│   └── **myrpc**
├── main
│   ├── CMakeLists.txt
│   ├── config_wifi.cpp
│   ├── config_wifi.hpp
│   ├── erpc_matrix_multiply.h
│   ├── erpc_matrix_multiply_server.h
│   ├── server.cpp
│   └── server_stub.cpp
├── partitions_example.csv
├── pytest_import_lib.py
├── sdkconfig
├── sdkconfig.defaults
└── sdkconfig.old
```

2. 在项目main目录的CMakeLists.txt中注册对myrpc的依赖
```cmake
idf_component_register(SRCS "server.cpp" "server_stub.cpp" "config_wifi.cpp"
                       INCLUDE_DIRS "."
                       REQUIRES myrpc esp_wifi nvs_flash)
```

接下来，使用idf框架的编译工具链正常编译main目录即可。

## 使用
这部分的内容参考example。example中原本包含一个矩阵乘法的例子，但是后来因为过于复杂没有继续使用，而是用了erpc_test，其包含三个int32参数，两个in一个out，可以在server端灵活实现多种测试功能。

### 代码生成
编写一个.erpc文件，其语法参考见[erpc文档](https://github.com/EmbeddedRPC/erpc/wiki/IDL-Reference)
然后,使用编译好的代码生成工具erpcgen对.erpc文件进行编译。对.erpc编译后会生成对应的源文件和头文件, 参考example中的Makefile:
```make
ERPC=idl.erpc
ERPC_SERVER=erpc_matrix_multiply_server.cpp 
ERPC_CLIENT=erpc_matrix_multiply_client.cpp
ERPC_HEADER=erpc_matrix_multiply_server.hpp \
			erpc_matrix_multiply.h
${ERPC_SERVER} ${ERPC_CLIENT} ${ERPC_HEADER}:
	erpcgen ${ERPC} -g c
```

### 使用生成的代码
代码生成工具已经进行了修改,生成的代码内容不能完全参考erpc文档.
`erpc_matrix_multiply_client.cpp` 和 `erpc_matrix_multiply.h`构成client编译单元, 包含一个`MatrixMultiplyServiceClient`类, 即client端代理.

`erpc_matrix_multiply_server.cpp`和`erpc_matrix_multiply_server.h`构成server编译单元, 包含一个`MatrixMultiplyService_service`类,即server端代理.

编写client和server函数时要分别包含上述两个编译单元。

### client端

包含头文件

```c++
#include "rpc_client.h"			  //位于myrpc/infra/client中
#include "erpc_matrix_multiply.h" //生成的代码
```

初始化MessageBufferFactory 

```c++
/* MessageBufferFactory initialization */
    erpc::MessageBufferFactory *message_buffer_factory = new erpc::MessageBufferFactory();
```

初始化client，需要传递ip地址，端口号，以及上一步创建的工厂类。

```c++
 /* eRPC client side initialization */
    MatrixMultiplyServiceClient *client = new MatrixMultiplyServiceClient("192.168.0.101", 12345, message_buffer_factory);
```

调用client的open函数，尝试连接服务器
```c++
    if (rpc_status::Success != client->open()) return -1;
```

链接成功后使用erpc函数，就如同使用一个普通的client成员函数一样。
```c++
    for (;;)
    {
        client->erpctest(num1, num2, &ret);
        printf("response: %" PRId32 "\n", ret);
        usleep(2000000);
    }
```

### server端
包含头文件

```c++
#include "erpc_matrix_multiply_server.h"	// 生成的文件
#include "simple_server.hpp"				// 位于myrpc/infra/server中
```

初始化MessageBufferFactory 

```c++
/* MessageBufferFactory initialization */
    auto message_buffer_factory = new erpc::MessageBufferFactory();
```

初始化server，需要传递ip地址，端口号，以及上一步创建的工厂类。

```c++
auto server = new erpc::SimpleServer("localhost", 12345, message_buffer_factory);
```

调用创建新的service并将其注册到server中。
```c++
	auto service = new MatrixMultiplyService_service();
    service->setName("Test Service");
    server->addService(service);
```

实现函数，注意idl中定义的方法是在service中的，但是实现方法时不要将他们实现为service的成员函数。
```c++
/* implementation of function call */
void erpcMatrixMultiply(Matrix matrix1, Matrix matrix2, Matrix result_matrix)
{
...
}

void erpctest(int32_t num1, int32_t num2, int32_t *ret) {
...
}

int main()
{
...
}
```

启动服务器，从这一步开始可以建立网络连接
```c++
server->open();
```

### 编译代码
编译时，需要将client/server对应的编译单元与client/server一同编译。详情参考example下的Makefile
