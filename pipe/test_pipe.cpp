#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main() {
	int n, fd[2];
	pid_t pid;
	char line[4096];

	if (pipe(fd) < 0) {
		fprintf(stderr, "pipe error!\n");
		exit(1);
	}

	if ((pid = fork()) < 0) {
		fprintf(stderr, "fork error!\n");	
		exit(1);
	}
	else if (pid > 0) {
	/* parent */	
		close(fd[0]); /* 关闭父进程读管道 */
		write(fd[1], "Hello World\n", 12); /* 向管道中写入数据 */
	}
	else {
	/* child */	
		close(fd[1]); /* 关闭子进程写入管道 */
		n = read(fd[0], line, 4096); /* 从管道中读取数据 */
		write(STDOUT_FILENO, line, n);
	}

	exit(0);
}
