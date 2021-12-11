/*멀티쓰레드 기반 서버에 접속하는 클라이언트 프로그램*/
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
#include<stdio.h>

#define PORTNUM 9000//mh
#define NAME_SIZE 20
#define BUF_SIZE 256

void *send_msg(void* arg);
void *recv_msg(void* arg);
void error_handling(char* msg);


int main(int argc, char* argv[])
{
  int sock;
  struct sockaddr_in serv_addr; //서버주소정보 담을변수
  pthread_t snd_thread, rcv_thread; //쓰레드 2개 선언:1-보낼때, 2-받을 때
  void * thread_return;
  

  sock=socket(PF_INET, SOCK_STREAM,0);

  memset(&serv_addr,0,sizeof(serv_addr));
  serv_addr.sin_family=AF_INET;
  serv_addr.sin_addr.s_addr=inet_addr("192.168.19.141");//mh ip
  serv_addr.sin_port=htons(PORTNUM);

  if(connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr))==1)
    error_handling("connect() error!"); 

  /*쓰레드 생성 및 join*/
  pthread_create(&snd_thread, NULL, send_msg, (void*)&sock);
  pthread_create(&rcv_thread, NULL, recv_msg, (void*)&sock);
  pthread_join(snd_thread, &thread_return);
  pthread_join(rcv_thread, &thread_return);
  close(sock);
  return 0;
}

void* send_msg(void* arg) //send thread main
{
  int sock=*((int*)arg);
  char name_msg[NAME_SIZE+BUF_SIZE];
  char msg[BUF_SIZE]; //mh
  char name[NAME_SIZE]; //mh
      
    while(1)
    {
      fgets(msg, BUF_SIZE, stdin);

      if(!strcmp(msg,"q\n")||!strcmp(msg,"Q\n"))//q ,Q 입력 시 socket을 닫고 채팅 종료
	{
	  close(sock);
	  exit(0);
	}
      send(sock,name,sizeof(name),0);//경매신청
      
      sprintf(name_msg, "[%d] %s", sock, msg);//[이름] 채팅내용
     
      send(sock, name_msg, sizeof(name_msg),0);
      
      name_msg[0]='\0';
      msg[0]='\0';
      name[0]='\n';
      
    }
  return NULL;

}

void* recv_msg(void* arg) //recv thread의 main
{
  int sock=*((int*)arg);
  char name_msg[NAME_SIZE+BUF_SIZE];

  int len;
  while(1){
    len=recv(sock, name_msg,sizeof(name_msg),0);//읽어오기
 
    if(len==-1)//오류가 있다면
	return (void*)-1;

    fputs(name_msg,stdout);//출력
    name_msg[0]='\0';//전에 버퍼에 들어있던 내용 삭제
  }
  return NULL;
}
void error_handling(char* msg)
{
  printf("%s",msg);
  exit(1);
}
