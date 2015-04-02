#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>

#define FILENAME "filefortest"

int lock_reg(int, int, int, off_t, int, off_t);

#define read_lock(fd, offset, whence, len) \
	lock_reg((fd), F_SETLK, F_RDLCK, (offset), (whence), (len))
#define readw_lock(fd, offset, whence, len) \
	lock_reg((fd), F_SETLKW, F_RDLCK, (offset), (whence), (len))
#define write_lock(fd, offset, whence, len) \
	lock_reg((fd), F_SETLK, F_WRLCK, (offset), (whence), (len))
#define writew_lock(fd, offset, whence, len) \
	lock_reg((fd), F_SETLKW, F_WRLCK, (offset), (whence), (len))
#define un_lock(fd, offset, whence, len) \
	lock_reg((fd), F_SETLK, F_UNLCK, (offset), (whence), (len))

int main() {
	int fd;

	if ((fd = open(FILENAME, O_RDWR)) < 0) {
		printf("%s is not exist!\n", FILENAME);	
		exit(1);
	}

	write_lock(fd, 0, SEEK_SET, 0);

	sleep(100);

	exit(0);
}

int lock_reg(int fd, int cmd, int type, off_t offset, int whence, off_t len) {

	struct flock lock;
	memset(&lock, 0x00, sizeof(struct flock));
	lock.l_type = type;
	lock.l_start = offset;
	lock.l_whence = whence;
	lock.l_len = len;

	return (fcntl(fd, cmd, &lock));
}
