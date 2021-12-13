/*硫��곗벐�덈뱶 湲곕컲 �쒕쾭 �꾨줈洹몃옩*/
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

#define PORTNUM 8881 //port吏���
#define MAX_CLNT 20

void* handle_client(void *arg); //�묒냽�섎뒗 �ъ슜�먮� 愿�由ы븯�� thread main
void send_msg(int ns,char* buf,int len); //�ъ슜�먮뱾�먭쾶 硫붿꽭吏� �꾨떖
void send_msg_all(char* buf,int len);

int client_cnt=0;
int clients[MAX_CLNT]; //�대씪�댁뼵�� �뚯폆
int MAX_PRICE=0; //理쒖쥌寃쎈ℓ媛�

pthread_mutex_t mutx;
pthread_cond_t rw_threadshold_cv;

int main(void) {
  char buf[256];
  struct sockaddr_in sin, cli;
  int sd, ns, clientlen = sizeof(cli);
  pthread_t pthread[20];

  //struct tm *t;
  //time_t timer = time(NULL);
  //t=localtime(&timer);

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

  if (listen(sd,3)) {
    perror("listen");
    exit(1);
  }

  while(1){
    //t=localtime(&timer);

    if ((ns = accept(sd, (struct sockaddr *)&cli, &clientlen))==-1){
      perror("accept");
      exit(1);
    }

    pthread_mutex_lock(&mutx);
    clients[client_cnt++]=ns;
    pthread_mutex_unlock(&mutx);

    pthread_create(&pthread[client_cnt-1],NULL,handle_client,(void*)&ns);
    for(int j=0;j<client_cnt;j++){
      pthread_detach(pthread[j]);
    }
  }
  close(sd);

  return 0;
}
void *handle_client(void *arg){//�ъ슜�� �쒕퉬�� �쒓났 thread main
  int ns=*((int*)arg);
  int i,len;
  char name[20];
  char buf[256];
  char name_buf[300];
  int price=0;
  
  if((len=recv(ns,name,sizeof(name),0))!=0)
    fprintf(stderr,"ok,%s",name);
    
  if((len=recv(ns,buf,sizeof(buf),0))!=0)
    {
      price=atoi(buf);
      
      if(price>MAX_PRICE)
      { fprintf(stderr,"big\n");
        
        pthread_mutex_lock(&mutx);
        MAX_PRICE=price;
        pthread_mutex_unlock(&mutx);
        
	name[strlen(name)-1]='\0'; 
	sprintf(name_buf,"current MAX : [%s] %s", name, buf); //현재 낙찰가

        pthread_mutex_lock(&mutx);
        send_msg_all(name_buf,strlen(name_buf)); //모두에게 방송
        pthread_mutex_unlock(&mutx);
	
	
        memset(name_buf,'\0',sizeof(name_buf));
        memset(buf,'\0',sizeof(buf));
      }
      else
	{
	fprintf(stderr,"small\n");
	//memset(buf,'\0',sizeof(buf));
	strcpy(buf,"\nPlz call the higher price.*\n");
	send_msg(ns,buf,strlen(buf));
	memset(buf,'\0',sizeof(buf));
      }
    }
       
  while((len=recv(ns,buf,sizeof(buf),0))!=0) {
       price = atoi(buf);

    if(price>MAX_PRICE)
      { fprintf(stderr,"big\n");
        sprintf(name_buf,"current MAX : [%s] %s", name, buf);
        
        pthread_mutex_lock(&mutx);
	MAX_PRICE=price;	        
	send_msg_all(name_buf,strlen(name_buf)); //모두에게 방송
        pthread_mutex_unlock(&mutx);
        
        memset(name_buf,'\0',sizeof(name_buf));
      }
    else
      {
	fprintf(stderr,"small\n");
	strcpy(buf,"\n Plz call the higher price.\n");
	send_msg(ns,buf,strlen(buf));	//클라이언트에게 방송
      }
    memset(buf,'\0',sizeof(buf));
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

void send_msg_all(char* buf,int len)
{
  for(int i=0; i<client_cnt;i++)
    {
      send_msg(clients[i],buf,len);
     }
      
}
