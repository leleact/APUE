#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
static void sig_quit(int);
int main()
{
	sigset_t newmask, oldmask, pendmask;

	if (signal(SIGQUIT, sig_quit) == SIG_ERR)
	{
		fprintf(stderr, "can't catch SIGQUIT\n");
		exit(1);
	}

	sigemptyset(&newmask);
	sigaddset(&newmask, SIGQUIT);

	if (sigprocmask(SIG_BLOCK, &newmask, &oldmask) < 0)
	{
		fprintf(stderr, "SIG_BLOCK error\n");
		exit(1);
	}

	sleep(5); // 此时^\信号发送到进程被屏蔽，进程会继续向下执行到结束

	// exit(0);

	if (sigpending(&pendmask) < 0)
	{
		fprintf(stderr, "sigpending error\n");	
		exit(1);
	}
	if (sigismember(&pendmask, SIGQUIT))
	{
		printf("\nSIGQUIT pending\n");	
	}

	if (sigprocmask(SIG_SETMASK, &oldmask, NULL) < 0)
	{
		fprintf(stderr, "SIG_SETMASK error\n");	
		exit(1);
	}

	printf("SIGQUIT unblocked\n");

	sleep(5);

	exit(0);
}

static void sig_quit(int signo)
{
	printf("caught SIGQUIT\n");
	if (signal(SIGQUIT, SIG_DFL) == SIG_ERR)
	{
		fprintf(stderr, "can't reset SIGQUIT\n");
		exit(1);
	}
}
