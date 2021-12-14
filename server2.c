
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

#define PORTNUM 9005 //port吏���
#define MAX_CLNT 20

void* handle_client(void *arg);
void send_msg(int ns,char* buf,int len);
void send_msg_all(char* buf,int len);
void handler(int signo);

int client_cnt=0;
int clients[MAX_CLNT]; 
int MAX_PRICE=0; //

typedef struct _client{ //구매자
  int ns;
  int max;
} client;

client cons; // 최대 경매가를 부른 사람

pthread_mutex_t mutx;

int main(void) {
  char buf[256];
  struct sockaddr_in sin, cli;
  int sd, ns, clientlen = sizeof(cli);
  
  pthread_t pthread[5];

  // struct sigaction act;

  pthread_mutex_init(&mutx, NULL);
 
  if ((sd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
    perror("socket");
    exit(1);
  }
  
  memset((char *)&sin, '\0', sizeof(sin));
  sin.sin_family = AF_INET;
  sin.sin_port = htons(PORTNUM);
  sin.sin_addr.s_addr = inet_addr("192.168.19.141");//mh ip

  if (bind(sd, (struct sockaddr *)&sin, sizeof(sin))) {    
    perror("bind");
    exit(1);
  }

  if (listen(sd,5)) {
    perror("listen");
    exit(1);
  }

  while(1){
    if ((ns = accept(sd, (struct sockaddr *)&cli, &clientlen))==-1){
      perror("accept");
      exit(1);
    }

    pthread_mutex_lock(&mutx);
    clients[client_cnt++]=ns;
    pthread_mutex_unlock(&mutx);

    pthread_create(&pthread[client_cnt-1],NULL,handle_client,(void*)&ns);
    
  }
  for(int j=0;j<client_cnt;j++){
   pthread_detach(pthread[j]); 
  }
  //fprintf(stderr,"max:%d\n",cons.max);
  
  pthread_exit(0);
  close(sd);

  return 0;
}

void *handle_client(void *arg){
  int ns=*((int*)arg);
  int i,len;
  char name[20];
  char buf[256];
  char name_buf[256];
  int price=0;

   
  if((len=recv(ns,name,sizeof(name),0))!=0){
    fprintf(stderr,"ok,%s",name);
    name[strlen(name)-1]='\0';
    if((len=recv(ns,buf,sizeof(buf),0))!=0){
        price=atoi(buf); //제시 가격
      
        if(price>MAX_PRICE)
          {
            fprintf(stderr,"max price:%d\n",price);
        
            pthread_mutex_lock(&mutx);
          
            MAX_PRICE=price;
            cons.ns=ns;
            cons.max=MAX_PRICE;

            sprintf(name_buf,"current MAX : [%s] %s", name, buf); //현재 낙찰가;
            send_msg_all(name_buf,strlen(name_buf)); //모두에게 현재 낙찰가격 방송
            
            pthread_mutex_unlock(&mutx);
	
	
            memset(name_buf,'\0',sizeof(name_buf));
            memset(buf,'\0',sizeof(buf));
          }else{
            fprintf(stderr,"low\n");
            memset(buf,'\0',sizeof(buf));
        
            strcpy(buf,"\nPlz call the higher price!!\n");
        
            send_msg(ns,buf,strlen(buf));

            memset(name_buf,'\0',sizeof(name_buf));
            memset(buf,'\0',sizeof(buf));
          }
   
        while((len=recv(ns,buf,sizeof(buf),0))!=0) {
          price = atoi(buf);
          if(price>MAX_PRICE)
            {
              fprintf(stderr,"max price:%d\n",price);
              sprintf(name_buf,"current MAX : [%s] %s", name, buf); 

              pthread_mutex_lock(&mutx);
           
              MAX_PRICE=price;
              cons.ns=ns;
              cons.max=MAX_PRICE;
           	        
              send_msg_all(name_buf,strlen(name_buf)); //모두에게 방송
           
              pthread_mutex_unlock(&mutx);
        
              memset(name_buf,'\0',sizeof(name_buf));
              memset(buf,'\0',sizeof(buf));
            }else{
              fprintf(stderr,"low\n");
              memset(buf,'\0',sizeof(buf));
            
              strcpy(buf,"\nPlz call the higher price!!\n");
              send_msg(ns,buf,strlen(buf));
            
              memset(buf,'\0',sizeof(buf));
              memset(name_buf,'\0',sizeof(name_buf));
            }
        }
      }
  
  }
  close(ns);
}

void send_msg(int ns,char* buf, int len)
{
  //int i;
  //pthread_mutex_lock(&mutx);
  send(ns, buf, len, 0);
  //pthread_mutex_unlock(&mutx);
}

void send_msg_all(char* buf,int len)
{
  for(int i=0; i<client_cnt;i++)
  {
      send_msg(clients[i],buf,len);
   }
      
}
