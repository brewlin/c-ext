# 基于c ，c ++ 封装php对象扩展
docs:http://wiki.brewlin.com/wiki/c-ext/index/
## 编译部署
```
$> phpize
$> ./configure
$> make & make install
$> echo extension=path/to/lib.so >> /etc/php?/php.ini
```
## @Lib
所有的扩展对象均以`Lib` 命名空间为开头
```php
namespace Lib;
```
## @`cgo()`
创建一个协程运行
## @`Lib/SharMem`
该扩展申请一块共享内存地址，提供php调用，用于多进程间共享数据
## @`Lib/Process`
该扩展初始化传入回调函数并创建子进程执行，子进程间可以通过channel通讯
## @`Lib/Timer`
提供定时任务和对于timer操作，基于epoll阻塞实现定时器，采用链表保存时间任务，有待提高性能
## @`Lib/Coroutine/Server`
提供携程化socket服务，监听tcp协议
## @`Lib/Thread/Pool` 
创建线程池,等待投递任务执行
## @`Lib/Thread/Pool/Future`
阻塞等待线程任务执行完毕，并获取返回值