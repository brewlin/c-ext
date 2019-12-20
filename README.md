# 基于c ，c ++ 封装php对象扩展
docs:http://docs.huido.site/wiki/c-ext/index/
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

## @`Lib/SharMem`
该扩展申请一块共享内存地址，提供php调用，用于多进程间共享数据
```php
//初始化传入内存大小 单位字节 bytes
$obj = new Lib\SharMem(8)

//模拟int 自增测试
$obj->get();
$obj->incr();
```
多进程共享内存测试,采用lib_fork 调用 原生fork操作
```php
$Obj = new Lib\SharMem(8);
if(lib_fork() == 0)
{
//子进程
    while(true)
    {
        sleep(1);
        $obj->incr();
        var_dump("this is clild process \n");
    }
}
//父进程
while(true)
{
    sleep(1);
    var_dump("this is parent process get value:".$obj->get());
}
```
## @`Lib/Process`
该扩展初始化传入回调函数并创建子进程执行，子进程间可以通过channel通讯
```php
<?php
$process = new Lib\Process(function(Lib\Process $process){
    while(true){
        $data = $process->read();
        echo "child process :> get parent msg: $data \n\n";
    }
});

$process->start();
for($i = 0;$i < 10 ; $i ++ ){
    echo "parent process :> send child msg: $i\n";
    $process->write($i);
    sleep(1);
}
```
- `construct()`
初始化构造函数时必须传入回调函数，在子进程创建时会调用
- `start()`
执行创建子进程操作
- `$process->write($data)`
向子进程或者父进程写入数据
- `$process->read()`
向子进程或父进程读取数据

- `$process->getpid()`
获取当前进程id
-  `$process->getppid()`
获取父进程id