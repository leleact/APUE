#include <iostream>
#include <cstring>
#include <cstdlib>
#include <memory.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/fcntl.h>
#include <sys/shm.h>

#define ARRAY_SIZE	40000
#define MALLOC_SIZE 100000
#define SHM_SIZE	100000
#define SHM_MODE	0600

char array[ARRAY_SIZE];

int main(void) {
	int shmid;
	int *ptr, *shmptr;

	printf("array[] from %lx to %lx\n", (unsigned long)&array[0],
			(unsigned long)&array[ARRAY_SIZE]);

	printf("stack around %lx\n", (unsigned long)&shmid);

	if ((ptr = (int *)malloc(MALLOC_SIZE)) == nullptr) {
		std::cerr << "malloc error\n";
		exit(1);
	}
	printf("malloc from %lx to %lx\n", (unsigned long)ptr, (unsigned long)ptr + MALLOC_SIZE);

	if ((shmid = shmget(IPC_PRIVATE, SHM_SIZE, SHM_MODE)) < 0) {
		std::cerr << "shmget error\n";
		exit(1);
	}

	if ((shmptr = (int *)shmat(shmid, 0, 0)) == (void *)-1) {
		std::cerr << "shmat error\n";
		exit(1);
	}

	printf("shared memory attached from %lx to %lx\n",
			(unsigned long)shmptr, (unsigned long)shmptr + SHM_SIZE);

//	pause(); //Ctrl-C进程结束后，ipcs -m 查看共享内存不会消失，再次运行会得到不同的shmid的共享内存

	if (shmctl(shmid, IPC_RMID, 0) < 0) {
		std::cerr << "shmctl error\n";
		exit(1);
	}
	exit(0);
}
