#include <iostream>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
using namespace std;
#define MAX_EVENTS 500
struct MyEvents {
	int fd;
	void (*pfun_call_back)(int fd, int events, void *arg);
	int events;
	void *arg;
	int status;
	char strBuff[128 + 1];
	int len;
	long last_active;
};

void EventSet(MyEvents *ev, int fd, void (*pfun_call_back)(int, int , void *), void *arg) {
	ev->fd = fd;
	ev->pfun_call_back = pfun_call_back;
	ev->events = 0;
	ev->arg = arg;
	ev->status = 0;
	ev->last_active = time(NULL);
}

void EventAdd(int epollfd, int events, MyEvents *ev) {
	struct epoll_event epv = {0, {0}};
	int op;
	epv.data.ptr = ev;
	epv.events = ev->events = events;
	if (ev->status == 1) {
		op = EPOLL_CTL_MOD;
	}
	else {
		op = EPOLL_CTL_ADD;
		ev->status = 1;
	}

	if (epoll_ctl(epollfd, op, ev->fd, &epv) < 0) {
		printf("Event Add Failed[fd = %d]\n", ev->fd);
	}
	else
		printf("Event Add OK[fd = %d]\n", ev->fd);
}

void EventDel(int epollfd, MyEvents *ev) {
	struct epoll_event epv = {0, {0}};
	if (ev->status != 1) return;
	epv.data.ptr = ev;
	ev->status = 0;
	epoll_ctl(epollfd, EPOLL_CTL_DEL, ev->fd, &epv);
}

int g_epollfd;
MyEvents g_Events[MAX_EVENTS + 1];
void RecvData(int fd, int events, void *arg);
void SendData(int fd, int events, void *arg);

void AcceptConn(int fd, int events, void *arg) {
	struct sockaddr_in sin;
	socklen_t len = sizeof(struct sockaddr_in);
	int nfd, i;
	if ((nfd = accept(fd, (struct sockaddr *)&sin, &len)) == -1) {
		if (errno == EAGAIN && errno != EINTR) {
			printf("%s: bad accept\n", __func__);
		}
		return;
	}


	do {
		for (i = 0; i< MAX_EVENTS; ++i) {
			if (g_Events[i].status == 0)
				break;
		}

		if (i == MAX_EVENTS) {
			printf("%s: max connection limit[%d]\n", __func__, MAX_EVENTS);
			break;
		}

		if (fcntl(nfd, F_SETFL, O_NONBLOCK) < 0) break;

		EventSet(&g_Events[i], nfd, RecvData, &g_Events[i]);
		EventAdd(g_epollfd, EPOLLIN|EPOLLET, &g_Events[i]);
		printf("new conn[%s:%d][time:%ld\n]", inet_ntoa(sin.sin_addr), ntohs(sin.sin_port), g_Events[i].last_active);
	}
	while(0);
}

void RecvData(int fd, int events, void *arg) {
	struct MyEvents *ev = (struct MyEvents *)arg;
	int len;
	len = recv(fd, ev->strBuff, sizeof(ev->strBuff) -1, 0);
	EventDel(g_epollfd, ev);

	if (len > 0) {
		ev->len = len;
		ev->strBuff[len] = '\0';
		printf("C[%d]:%s\n", fd, ev->strBuff);

		EventSet(ev, fd, SendData, ev);
	}
	else if (len == 0) {
		close(ev->fd);
		printf("[fd = %d] closed gracefully\n", fd);
	}
	else {
		close(ev->fd);
		printf("recv[fd=%d], error[%d]:%s\n", fd, errno, strerror(errno));
	}
}

void SendData(int fd, int events, void *arg) {
	struct MyEvents *ev = (struct MyEvents *)arg;
	int len;
	len = send(fd, ev->strBuff, ev->len, 0);
	ev->len = 0;
	EventDel(g_epollfd, ev);
	if (len > 0) {
		EventSet(ev, fd, RecvData, ev);
		EventAdd(g_epollfd, EPOLLIN|EPOLLET, ev);
	}
	else {
		close(ev->fd);
		printf("recv[fd=%d] error[%d]\n", fd, errno);
	}
}

void InitListenSocket(int epollfd, short port) {
	int listenfd = socket(AF_INET, SOCK_STREAM, 0);
	fcntl(listenfd, F_SETFL, O_NONBLOCK);
	printf("server listen: %d\n", listenfd);

	EventSet(&g_Events[MAX_EVENTS], listenfd, AcceptConn, &g_Events[MAX_EVENTS]);

	EventAdd(epollfd, EPOLLIN|EPOLLET, &g_Events[MAX_EVENTS]);

	sockaddr_in sin;
	bzero(&sin, sizeof(sin));
	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = INADDR_ANY;
	sin.sin_port = htons(port);
	bind(listenfd, (struct sockaddr *)&sin, sizeof(sin));
	listen(listenfd, 5);
}

int main(int argc, char **argv) {
	short port  = 8080;
	if (argc == 2){
		port = atoi(argv[1]);
	}

	g_epollfd  = epoll_create(MAX_EVENTS);
	if (g_epollfd <= 0) {
		printf("create epoll failed %d\n", g_epollfd);
		exit(1);
	}

	InitListenSocket(g_epollfd, port);
	struct epoll_event events[MAX_EVENTS];
	printf("server running : port[%d]\n", port);
	int checkPos = 0;
	while(1) {
		long now = time(NULL);
		for (int i = 0; i < 100; i++, checkPos++) {
			if (checkPos == MAX_EVENTS)	 checkPos = 0;
			if (g_Events[checkPos].status != 1) continue;
			long duration  = now - g_Events[checkPos].last_active;
			if (duration >= 60) {
				close(g_Events[checkPos].fd);
				printf("[fd=%d] timeout[%ld--%ld]\n", g_Events[checkPos].fd, g_Events[checkPos].last_active, now);
				EventDel(g_epollfd, &g_Events[checkPos]);
			}
		}

		int fds = epoll_wait(g_epollfd, events, MAX_EVENTS, 1000);
		if (fds < 0) {
			printf("epoll wait error\n");
			break;
		}

		for (int i = 0; i < fds; i++) {
			MyEvents *ev = (struct MyEvents *)events[i].data.ptr;
			if ((events[i].events&EPOLLIN)&&(ev->events&EPOLLIN)) {
				ev->pfun_call_back(ev->fd, events[i].events, ev->arg);	
			}

			if ((events[i].events&EPOLLOUT)&&(ev->events&EPOLLOUT)) {
				ev->pfun_call_back(ev->fd, events[i].events, ev->arg);	
			}
		}
	}

	return 0;
}
