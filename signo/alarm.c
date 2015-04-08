#include <stdio.h>
#include <unistd.h>
#include <signal.h>
int main() {
	alarm(10);
	sleep(11);
	return 0;
}
