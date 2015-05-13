#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

void str_cli(FILE *, int);

int main(int argc, char **argv) {
  int sockfd;
  int ret = 0;
  struct sockaddr_in servaddr;

  if (argc != 3) {
    fprintf(stdout, "usage: clien <IP address> <IP port>\n");
    exit(1);
  }

  int ip_port = atoi(*(argv + 2));

  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (-1 == sockfd) {
    fprintf(stderr, "socket error\n");
    exit(1);
  }

  bzero(&servaddr, sizeof(servaddr));
  servaddr.sin_family = AF_INET;
  servaddr.sin_port = htons(ip_port);
  inet_pton(AF_INET, argv[1], &servaddr.sin_addr);

  ret = connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr));
  if (-1 == ret) {
    fprintf(stderr, "connect error\n");
    exit(1);
  }

  str_cli(stdin, sockfd);

  exit(0);
}

void str_cli(FILE *fp, int sockfd) {
  char sendline[4096];
  char recvline[4096];

  while(fgets(sendline, 4096, fp) != NULL) {
    write(sockfd, sendline, strlen(sendline));

    memset(recvline, 0x00, sizeof(recvline));
    if (read(sockfd, recvline, 4096) == 0) {
      fprintf(stderr, "error: server terminated prematurely\n");
      exit(1);
    }

    fputs(recvline, stdout);
  }
}
