#记录锁


``` c
#include <fcntl.h>
int fcntl(int filedes, int cmd, .../* struct flock *flockptr */);
```
返回值：成功则依赖于cmd, 出错则返回-1

``` c
struct flock {
	short l_type;	/* F_RDLCK, F_WRLCK, or F_UNLCK */
	off_t l_start;	/* offset in bytes, relative to l_whence */
	short l_whence; /* SEEK_SET, SEEK_CUR, or SEEK_END */
	off_t l_len;	/* length, in bytes; 0 means lock to EOF */
	pid l_pid;		/* returned with F_GETLK */
}
```
