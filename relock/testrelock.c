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
	char buff[4096 + 1];

	if ((fd = open(FILENAME, O_RDWR)) < 0) {
		printf("%s is not exist!\n", FILENAME);	
		exit(1);
	}

	memset(buff, 0x00, sizeof(buff));

	while(read(fd, buff, sizeof(buff) - 1) > 0) {
		fprintf(stdout, "%s", buff);
	}
	
	struct flock sFlock;
	memset(&sFlock, 0x00, sizeof(struct flock));
	sFlock.l_type = F_WRLCK;
	sFlock.l_start = 0;
	sFlock.l_whence = SEEK_SET;
	sFlock.l_len = 0;

	if (-1 == fcntl(fd, F_SETLKW, &sFlock)) {
		fprintf(stderr, "fcntl err!\n");	
		exit(-1);
	}

	sleep(100);

	exit(0);
}
