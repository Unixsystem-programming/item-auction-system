#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <pthread.h>

#define PORTNUM 9000
#define MAX_CLNT 256

void* handle_client(void *arg);
void send_msg(int ns,char* buf,int len);

int client_cnt=0;
int clients[MAX_CLNT]; //클라이언트 소켓
pthread_mutex_t mutx;

int main(void) {
  char buf[256];
  struct sockaddr_in sin, cli;
  int sd, ns, clientlen = sizeof(cli);
  //pthread_t pid;
  pthread_t pthread[3];

  //struct tm *t;
  //time_t timer = time(NULL);
  //t=localtime(&timer);

  pthread_mutex_init(&mutx, NULL);
    //소켓생성
  if ((sd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
    perror("socket");
    exit(1);
  }
  
  memset((char *)&sin, '\0', sizeof(sin));
  sin.sin_family = AF_INET;
  sin.sin_port = htons(PORTNUM);
  sin.sin_addr.s_addr = inet_addr("192.168.19.141");

  //소켓 이름 지정
  if (bind(sd, (struct sockaddr *)&sin, sizeof(sin))) {    
    perror("bind");
    exit(1);
  }

  //클라이언트 연결 기다림
  if (listen(sd,3)) {
    perror("listen");
    exit(1);
  }

  while(1){
    int pid=0;
    //t=localtime(&timer);

    //연결 요청 수락
    if ((ns = accept(sd, (struct sockaddr *)&cli, &clientlen))==-1){
      perror("accept");
      exit(1);
    }
    //동기화
    pthread_mutex_lock(&mutx);
  
    pthread[client_cnt++]=ns;

    pthread_mutex_unlock(&mutx);

    pthread_create(&pthread[client_cnt-1],NULL,handle_client,(void*)&ns);
    for(int j=0;j<client_cnt;j++){
      pthread_detach(pthread[j]);
    }
    //sprintf(buf, "Connected client IP : %s", inet_ntoa(cli.sin_addr));
  }
  close(sd);

  return 0;
}
void *handle_client(void *arg){
  int ns=*((int*)arg);
  int i,len;
  char buf[256];

  if((len=recv(ns,buf,sizeof(buf),0))!=0) fprintf(stderr,"ok,%s",buf);
  //신청자 저장
  buf[0]='\0';
  
  while((len=recv(ns,buf,sizeof(buf),0))!=0) {
    send_msg(ns,buf,len);
    buf[0]='\0';
  }
 
  close(ns);
  return NULL;
}
void send_msg(int ns,char* buf, int len)
{
  //int i;
  //pthread_mutex_lock(&mutx);

    send(ns, buf, len, 0);

    //pthread_mutex_unlock(&mutx);
}
