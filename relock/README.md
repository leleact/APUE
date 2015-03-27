#记录锁


``` c
#include <fcntl.h>
int fcntl(int filedes, int cmd, .../* struct flock *flockptr */);
```
返回值：成功则依赖于cmd, 出错则返回-1

cmd:
1. 复制一个现有的描述符(cmd = F\_DUPFD)
2. 获得/设置文件描述符标记(cmd = F\_GETFD或F\_SETFD)
3. 获得/设置文件状态标识(cmd = F\_GETFL或F\_SETFL)
4. 获得/设置异步I/O所有权(cmd = F\_GETOWN或F\_SETOWN)
5. 获得/设置记录锁(cmd = F\_GETLK、F\_SETLK或F\_SETLKW)

``` c
struct flock {
	short l_type;	/* F_RDLCK, F_WRLCK, or F_UNLCK */
	off_t l_start;	/* offset in bytes, relative to l_whence */
	short l_whence; /* SEEK_SET, SEEK_CUR, or SEEK_END */
	off_t l_len;	/* length, in bytes; 0 means lock to EOF */
	pid_t l_pid;	/* returned with F_GETLK */
};
```
