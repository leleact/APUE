#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
int main()
{
	pid_t pid;

	if (-1 == (pid = fork()))
	{
		fprintf(stderr, "fork error\n");	
		exit(1);
	}
	else if (pid == 0)
	{
		/* child */
		if (-1 == execle("strecho", "a", "b", "c", (char *)0))
		{
			fprintf(stderr, "execle error\n");	
			exit(1);
		}

		exit(1);
	}

	int status;
	waitpid(pid, &status, WSTOPPED);
	printf("status = [%d]\n", status);

	exit(0);
}
