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
#include<unistd.h>
#include<fcntl.h>
#define PORTNUM 9001
#define NAME_SIZE 20
#define BUF_SIZE 256

void *send_msg(void* arg);
void *recv_msg(void* arg);
void error_handling(char* msg);

pthread_mutex_t rw_mutex=PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t r_threadshold_cv=PTHREAD_COND_INITIALIZER;
pthread_cond_t w_threadshold_cv=PTHREAD_COND_INITIALIZER;


int main(int argc, char* argv[])
{
  int sock;
  struct sockaddr_in serv_addr; 
  pthread_attr_t attr;
  pthread_t snd_thread, rcv_thread; 
  void * thread_return;
  pthread_attr_init(&attr);
  pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);


  sock=socket(PF_INET, SOCK_STREAM,0);

  memset(&serv_addr,0,sizeof(serv_addr));
  serv_addr.sin_family=AF_INET;
  serv_addr.sin_addr.s_addr=inet_addr("210.93.57.50");//mh ip
  serv_addr.sin_port=htons(PORTNUM);

  if(connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr))==1)
    error_handling("connect() error!"); 


  pthread_create(&snd_thread,&attr, send_msg, (void*)&sock); //송신 쓰레드 생성
  pthread_create(&rcv_thread,&attr, recv_msg, (void*)&sock); //수신 쓰레드 생성
  pthread_join(snd_thread, &thread_return);
  pthread_join(rcv_thread, &thread_return);

  pthread_attr_destroy(&attr);

  pthread_exit (0);
  close(sock);
  
  return 0;
}

void* send_msg(void* arg) //send thread main
{
  int sock=*((int*)arg);

  char msg[BUF_SIZE]; //mh
  char name[NAME_SIZE]; //mh
  char buf[NAME_SIZE];


  pthread_mutex_lock(&rw_mutex);

  fprintf(stderr,"Plz put your name : "); //이름입력
  fgets(name,NAME_SIZE,stdin);
  send(sock,name,sizeof(name),0);
  
  name[strlen(name)-1]='\0';
  
  while(1){
    memset(msg,'\0',sizeof(msg));

    pthread_cond_signal(&r_threadshold_cv); //수신 쓰레드 깨우고 
    pthread_cond_wait(&w_threadshold_cv, &rw_mutex); //수신 쓰레드가 메세지를 입력받고 깨워주길 기다림
    
    fprintf(stderr,"\nEnter the Price : ");
    fgets(msg, BUF_SIZE, stdin);
    msg[strlen(msg)-1]='\0';
    send(sock, msg, sizeof(msg),0);
    
    pthread_mutex_unlock(&rw_mutex);
    pthread_mutex_lock(&rw_mutex);    //
  }
   pthread_exit (NULL);
  return NULL;

}

void* recv_msg(void* arg) //recv thread�� main
{
  int sock=*((int*)arg);
  char name_msg[NAME_SIZE+BUF_SIZE];
  int len;
  
  sleep(0.5); //송신 쓰레드가 먼저 실행되게 하기위함
  while(1){
    
    pthread_mutex_lock(&rw_mutex);
    pthread_cond_signal(&w_threadshold_cv);
    pthread_cond_wait(&r_threadshold_cv, &rw_mutex);
    len=recv(sock, name_msg,sizeof(name_msg),0);
    if(name_msg[0]=='R')
      {
	char* fn = "item2.txt";
	int fd=open(fn,O_WRONLY|O_CREAT|O_APPEND);
	recv(sock,name_msg,sizeof(name_msg),0);
	write(fd,name_msg,strlen(name_msg));
	memset(name_msg,0,sizeof(name_msg));
	exit(1);
      }
    fprintf(stderr,name_msg,len);
    pthread_mutex_unlock(&rw_mutex);

    memset(name_msg,'\0',sizeof(name_msg));
  }
  pthread_exit (NULL);
  return NULL;
}

void error_handling(char* msg)
{
  printf("%s",msg);
  exit(1);
}
