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
void send_msg(char* buf,int len);

int client_cnt=0;
int clients[MAX_CLNT]; //클라이언트 소켓
pthread_mutex_t mutx;

int main(void) {
  char buf[256];
  struct sockaddr_in sin, cli;
  int sd, ns, clientlen = sizeof(cli);
  pthread_t pid;
 

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
  if (listen(sd,5)) {
    perror("listen");
    exit(1);
  }

  while(1){
    //t=localtime(&timer);

    //연결 요청 수락
    if ((ns = accept(sd, (struct sockaddr *)&cli, &clientlen))==-1){
      perror("accept");
      exit(1);
    }
    //동기화
    pthread_mutex_lock(&mutx);
    clients[client_cnt++]=ns;
    pthread_mutex_unlock(&mutx);

    pthread_create(&pid,NULL,handle_client,(void*)&ns);
    pthread_detach(pid);
    //sprintf(buf, "Connected client IP : %s", inet_ntoa(cli.sin_addr));
  }
  close(sd);

  return 0;
}
void *handle_client(void *arg){
  int ns=*((int*)arg);
  int i,len;
  char buf[256];

  while((len=read(ns,buf,sizeof(buf)-1))!=0) {
    //fprintf(stderr,"1.msg: %s\n",buf);
    //fprintf(stderr,"1.len: %d\n",len);
    //fprintf(stderr,"1.strlen: %ld\n----------",strlen(buf));
    send_msg(buf,len);
    //fprintf(stderr,"3.msg: %s",buf);

    buf[0]='\0';
    //flush(buf);
  }
  
  pthread_mutex_lock(&mutx);
  for(i=0;i<client_cnt;i++){
    if(ns==clients[i]){
      while(i++<client_cnt-1) clients[i]=clients[i]+1;
      break;
    }
  }
  client_cnt--;

  //sprintf(buf, "Your IP address is %s", inet_ntoa(cli.sin_addr));
  //sprintf(buf,"your cnt ..%d",ns);
  //if (send(ns, buf, strlen(buf) + 1, 0) == -1) {
  //perror("send");
  //exit(1);}
  pthread_mutex_unlock(&mutx);
  close(ns);
  return NULL;
}
void send_msg(char* buf, int len)
{
    int i;
    pthread_mutex_lock(&mutx);

    //send(clients[i], buf, len);
    for (i=0; i<client_cnt; i++)
      send(clients[i], buf, len, 0);
    //fprintf(stderr,"1.str_len: %ld\n",strlen(buf));
    //fprintf(stderr,"2.len: %d\n",len);
    
    //fprintf(stderr,"2.msg: %s",buf);
    
    pthread_mutex_unlock(&mutx);
}
