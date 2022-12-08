## RPC框架
### RPC改进
- 事件（轮询和多线程）全部转移到client/server，transport只负责纯粹的读写操作
- 去掉了奇怪的MANULLY_CONSTRCTED
  - 一组宏和模板配合实现的单例
  - 对这个单例可以用construct, deconstruct等完成构造和析构
  - 用opaque类型完全隐藏内部实现, 看上去就像是c程序
- 去掉了static_construct模式
  - buffer可以采用此方式构建, 如果采用此方式, 还会用上述的奇怪方式替换new delete等操作, 或许对内存管理有用吗?
- 去掉了最外层封装
  - 可以直接用new新建服务端/客户端, 调用其open, addservice方法等(算是节省一层没必要的封装吧)
- erpcgen定制
  - 去掉奇怪的g_client单例, 改用面向对象方式构建Client子类

### erpcgen原理
idl->astnode->info（嵌套的map）->templete

- 增加指定的属性，在对应的templete节点中往info里放入数据即可
- 改变文件结构，直接修改templete

### freeRTOS可行性研究
- 确定esp-idf支持c++
- malloc/free和task已经由erpc完成移植
- lwip接口基本和linux没有区别
- 但是lwip没有提供epoll, poll也只是用select实现的一个封装, 关于多路, 是否有必要实现?
- pthread似乎也在