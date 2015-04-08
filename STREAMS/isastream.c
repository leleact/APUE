/******************************************************************************/
/*                   STREAMS相关的函数在linux中似乎没有实现                   */
/*					 Date 2015-04-08										  */ 
/******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <stropts.h>
#include <unistd.h>
int isastream(int);
int main(int argc, char *argv[]) {
	int i, fd;
	for (i = 0; i < argc; i++) {
		if ((fd = open(argv[i], O_RDONLY)) < 0)	{
			fprintf(stderr, "%s: can't open\n", argv[i]);	
			continue;
		}

		if (isastream(fd) == 0) {
			fprintf(stdout, "%s: not a stream", argv[i]);
		}
		else {
			fprintf(stdout, "%s: streams device", argv[i]);
		}
	}

	exit(0);
}
int isastream(int fd) {
	return(ioctl(fd, I_CANPUT, 0) != -1);
}
