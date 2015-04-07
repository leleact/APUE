#STREAMS

	STREAMS的所有输入和输出都基于消息。流首和用户进程使用read write ioctl getmsg
getpmsg putmsg putpmsg交换消息。在流首和、各处理模块和设备驱动程序之间, 消息
可以顺流而下, 也可以逆流而上。
	在用户进程和流首之间, 消息由下列机部分组成：消息类型、可选择的控制信息以及可
选择的数据。表14-4列出了对应于write、putmsg和putpmsg的不同参数所产生的不同消息类
型。控制信息和数据由strbuf结构指定：
	
	``` c
	struct strbuf {
		int maxlen;			/* size of buffer */	
		int len;			/* number of bytes currently in buffer */
		char *buf;			/* pointer to buffer */
	}
	```

	当用putmsg或putpmsg发送消息时，len指定缓冲区中数据的字节数。当用getmsg或getpmsg
接收消息时，maxlen指定缓冲区长度（使内核不会溢出缓冲区）, 而len则由内核设置为存放在
缓冲区中的数据量。消息长度是0是允许的，len为-1说明没有控制信息或者数据。
