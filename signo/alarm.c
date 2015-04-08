#include <stdio.h>
#include <unistd.h>
#include <signal.h>
int main() {
	alarm(10);
//	for (;;);
	pause();
	return 0;
}
