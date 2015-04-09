#include <iostream>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MAXLINE 4096
#define DEF_PAGER "/bin/more" /* default pager program */

int main(int argc, char *argv[]) {
	int n;
	int fd[2];
	pid_t pid;
	char *pager, *argv0;
	char line[MAXLINE];
	FILE *fp;

	if (argc != 2) {
		fprintf(stderr, "usage: a.out <pathname>\n");	
		exit(1);
	}

	if ((fp = fopen(argv[1], "r")) == NULL) {
		fprintf(stderr, "can't open %s\n", argv[1]);	
		exit(1);
	}

	if (pipe(fd) < 0) {
		fprintf(stderr, "pipe error!\n");
		exit(1);
	}

	if ((pid = fork()) < 0) {
		fprintf(stderr, "fork error\n");	
		exit(1);
	} else if (pid > 0) {
	/* parent */	
		close(fd[0]); /* close read end */

		while(fgets(line, MAXLINE, fp) != NULL) {
			n  = strlen(line);
			if (write(fd[1], line ,n) != n) {
				fprintf(stderr, "write error to pipe\n");
				exit(1);
			}
		}

		if (ferror(fp)) {
			fprintf(stderr, "fgets error\n");
			exit(1);
		}

		close(fd[1]); /* close write end of pipe for reader */

		if (waitpid(pid, NULL, 0) < 0) {
			fprintf(stderr, "waitpid error\n");
			exit(1);
		}

		exit(0);
	} else {
		close(fd[1]); /* close write end */	
		if (fd[0] != STDIN_FILENO) {
			if (dup2(fd[0], STDIN_FILENO) != STDIN_FILENO) {
				fprintf(stderr, "dup2 error to stdin\n");	
				exit(1);
			}
			close(fd[0]);
		}
		/* get argumnets for execl */	
		if ((pager = getenv("PAGER")) == NULL)
			pager = DEF_PAGER;
		if ((argv0 = strrchr(pager, '/')) != NULL)
			argv0++; /* step past rightmost slash */
		else
			argv0 = pager; /* no slash in pager */

		if (execl(pager, argv0, (char *)0) < 0) {
			fprintf(stderr, "execl error for %s\n", pager);	
		}
	}
	
	exit(0);
}
