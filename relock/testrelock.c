#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#define FILENAME "filefortest"

int main() {
	int fd;
	char buff[4096 + 1];

	if ((fd = open(FILENAME, O_RDWR)) < 0) {
		printf("%s is not exist!\n", FILENAME);	
		exit(1);
	}

	memset(buff, 0x00, sizeof(buff));

}
