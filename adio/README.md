#五种I/O模型
1. 阻塞I/O
2. 非阻塞I/O
3. I/O复用/多路转发
4. 信号驱动I/O
5. 异步I/O(aio\_function())

##阻塞I/O

常规read, write函数均属于阻塞I/O, read或者write是, 程序不能向下执行, 必须等I/O操作结束之后才能操作, 比较耗费CPU资源。
![阻塞I/O](http://images.cnitblog.com/blog/468825/201403/082155536289553.png)
