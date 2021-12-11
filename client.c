#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>


#define PORTNUM 9001

int main(void) {
  char buf[256];
  struct sockaddr_in sin;
  int sd;

  //소켓생성
  if ((sd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
    perror("socket");
    exit(1);
  }

  memset((char *)&sin, '\0', sizeof(sin));
  sin.sin_family = AF_INET;
  sin.sin_port = htons(PORTNUM);
  sin.sin_addr.s_addr = inet_addr("192.168.19.141");

  //서버와 연결
  if (connect(sd, (struct sockaddr *)&sin, sizeof(sin))) {
    //fprintf(stderr,"connect!");
    perror("connect");
    exit(1);
  }

  if (send(sd, buf,sizeof(buf), 0) == -1) {
    perror("send");
    exit(1);
  }
  //데이터 받기
  if(recv(sd, buf, sizeof(buf), 0) == -1) {
    perror("recv");
    exit(1);
  }
  close(sd);
  printf("From Server : %s\n", buf);
  return 0;
}
