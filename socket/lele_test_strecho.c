#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define LISTENQ 5

void str_echo(int);

int main(int argc, char **argv) {
  int listenfd, connfd;
  pid_t childpid;
  socklen_t chilen;

  if (argc != 2) {
    fprintf(stdout, "please use : server <port>\n");
    exit(1);
  }

  int server_port = atoi(*(argv + 1));

  struct sockaddr_in cliaddr, servaddr;

  listenfd = socket(AF_INET, SOCK_STREAM, 0);
  if (-1 == listenfd) {
    fprintf(stderr, "socket error\n");
    exit(1);
  }

  bzero(&servaddr, sizeof(servaddr));
  servaddr.sin_family = AF_INET;
  servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
  servaddr.sin_port = htons(server_port);

  int ret = bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr));
  if (-1 == ret) {
    fprintf(stderr, "bind error\n");
    exit(1);
  }

  ret = listen(listenfd, LISTENQ);
  if (-1 == ret) {
    fprintf(stderr, "listen error\n");
    exit(1);
  }

  for (;;) {
    chilen = sizeof(cliaddr);
    connfd = accept(listenfd, (struct sockaddr *)&cliaddr, &chilen);
    if ((childpid = fork()) < 0) {
      fprintf(stderr, "fork error\n");
      exit(1);
    } else if (childpid == 0) {
      close(listenfd);
      str_echo(connfd);
      exit(0);
    }
    close(connfd);
  }
}

void str_echo(int sockfd) {
  ssize_t n;
  char buf[4096];

again:
  while((n = read(sockfd, buf, 4096)) > 0) {
    write(sockfd, buf, n);
  }

  if (n < 0 && errno == EINTR)
    goto again;
  else if (n < 0) {
    fprintf(stderr, "str_echo error\n");
    exit(1);
  }
}
