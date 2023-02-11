## 简介
代码可以在linux下单独运行，或者作为esp-idf框架中的component运行。
文件结构：
```bash
.
├── Debug                   ## makefile构建目录
├── build                   ## cmake构建目录
├── doc                     ## 文档
├── example                 ## 示例
├── infra                   ## 主体代码
└── mk                      ## makefile工具链
```
## 编译（linux）

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

### 代码生成
首先编写一个.erpc文件，其语法参考见[erpc文档](https://github.com/EmbeddedRPC/erpc/wiki/IDL-Reference)
编写完成后,使用erpcgen对其进行编译,
