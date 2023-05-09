# 将项目打包

将项目打包的方法是把项目代码作为esp-idf项目的一个component编译，然后在编译生成的文件中找到项目生成的静态库。为此，需要下载esp-idf框架，将项目代码用框架中的库实现。然后创建一个esp-idf项目并把代码放到component目录下，通过修改cmake进行注册，然后进行编译即可。
这个操作会将代码中的所有源文件打包成一个适用于esp32c3上FreeRTOS的.a文件。

## 安装esp-idf工具链
安装适用于**esp32c3**的ESP-IDF **v5.0**，参照[官方文档](https://docs.espressif.com/projects/esp-idf/zh_CN/release-v5.0/esp32c3/get-started/index.html#get-started-how-to-get-esp-idf)。
注意，ESP-IDF 编译系统不支持 ESP-IDF 路径或其工程路径中带有空格。
要使用idf.py，需要在VSCode中点击下方状态栏的小按钮，或者用`. $HOME/esp/esp-idf/export.sh`导入环境变量，其中export.sh位于安装路径。

## 将依赖的库改为esp-idf框架下的库
部分系统库的路径需要改为esp-idf框架中的路径，例如：
在FreeRTOS上：
```c
#include "FreeRTOS.h"
#include "task.h"
```
在**esp-idf**上：
```c
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
```

在标准C上：
```c
#include <netdb.h>
#include <sockets.h>
```
在**esp-idf**上
```c
#include <lwip/netdb.h>
#include <lwip/sockets.h>
```

## 适配C++代码
由于项目使用了C++，因此.c文件对应的头文件最好具有C的链接格式。即：
```c
#ifdef __cplusplus
extern "C" {
#endif
...
#ifdef __cplusplus
}
#endif
```

## 新建工程

参考官方的[编程指南](https://docs.espressif.com/projects/esp-idf/zh_CN/release-v5.0/esp32c3/get-started/linux-macos-setup.html#get-started-linux-macos-first-steps)，按照以下步骤：

1. 参考“开始创建工程”，但是将复制的目录改为 $IDF_PATH/examples/build_system/cmake/import_lib
2. 跳过“连接设备”
3. 参考“配置工程”，可以跳过imenuconfig

在vscode上，点击最下方的小方块图标“Open ESP-IDF Terminal”
然后：
```bash
cd espdir
cp -r $IDF_PATH/examples/build_system/cmake/import_lib # 如果这一步出错，检查是否是在VSCode ESP-IDF Terminal中进行操作，或是否进行了`. $HOME/esp/esp-idf/export.sh`导入环境变量
cd import_lib
idf.py set-target esp32c3 #这一步也可以通过VSCode底部状态栏实现
```

现在工程目录的结构应该如图所示：
```
.
├── CMakeLists.txt
├── README.md
├── build
├── components
│   └── tinyxml2
├── main
│   ├── CMakeLists.txt
│   ├── data
│   └── import_lib_example_main.cpp
├── partitions_example.csv
├── pytest_import_lib.py
├── sdkconfig
└── sdkconfig.defaults
```

## 将代码引入工程
假设所有的代码都在名为MyCode的文件夹下。我们要将其复制到工程的component目录下，在cmake中编写注册component的语句。然后修改main的内容，并且在main的cmake中注册

### 将代码复制到component

删除/component/tinyxml2

将MyCode复制到/component/MyCode

在/component/MyCode下新建一个CMakeList.txt，内容包括：
- 将项目的所有源文件赋值给一个变量，例如RPC_SRC.
- 将项目所有头文件所在的目录赋值给一个变量，例如RPC_INCLUDE. 
- 使用idf_component_register语句注册代码，REQUIRES的值为MyCode项目中依赖的所有esp-idf库。[详细语法参考](https://docs.espressif.com/projects/esp-idf/zh_CN/release-v5.0/esp32c3/api-guides/build-system.html#minimum-cmakelists)

下面是一个例子：
```cmake
file(GLOB_RECURSE RPC_SRC
        infra/*.cpp
        )
set(RPC_INCLUDE infra/
                infra/codec 
                infra/client 
                infra/server 
                infra/transport
                infra/port
                infra/port/threading
                infra/port/malloc_free
                infra/port/log )

idf_component_register(SRCS "${RPC_SRC}"
                       INCLUDE_DIRS "${RPC_INCLUDE}"
                       REQUIRES lwip)
```

### 修改main

修改/main/import_lib_example_main.cpp,将其内容删到只剩下一个空的主函数：
```cpp
extern "C" void app_main(void)
{

}

```

修改/main/CMakeLists.txt，在其中注册对MyCode的依赖:
```diff
idf_component_register(SRCS "import_lib_example_main.cpp"
                       INCLUDE_DIRS "."
-                       PRIV_REQUIRES tinyxml2 fatfs)
+                       REQUIRES MyCode)

# Create a FAT filesystem image from the contents of data/ subdirectory,
# The image will be flashed into the 'storage' partition when 'idf.py flash' is used.
- fatfs_create_spiflash_image(storage data FLASH_IN_PROJECT)
```

现在工程的结构应该如图所示,标*的内容是重点项目

```
.
├── CMakeLists.txt
├── README.md
├── components
│   └── *MyCode*
│           ├── *CMakeLists.txt*
│           └── infra
├── main
│   ├── *CMakeLists.txt*
│   ├── data
│   └── *import_lib_example_main.cpp*
├── partitions_example.csv
├── pytest_import_lib.py
├── sdkconfig
└── sdkconfig.defaults
```

## 编译代码

在ESP-IDF Terminal中：
```bash
idf.py build # 这一步也可以通过VSCode底部状态栏实现
cd build/esp/MyCode
```

应该能看到libMyCode.a， 这就是打包出的静态库，将这个静态库随头文件一起发布。

