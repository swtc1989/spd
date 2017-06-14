#include <sys/socket.h>
#include <iostream>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/signal.h>
#include <signal.h> 
#include <vector>
#include <map>
#include <stdio.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <utility>  
#define SIZE 3000

using namespace std;


#define N 3000
char cmdbuf[100];
int sockfd,n,id=0, result=0;

int current_state=0;

void print(void){

printf("Total visited counts: %d, and stop\n", id); 

exit(0);
}


int main(int argc, char**argv)
{
   signal(SIGINT , (void(*)(int)) print);
   struct sockaddr_in servaddr,cliaddr;
   socklen_t len;
   int states[SIZE]={0};
   char message[SIZE];
  // char state[10];
   sockfd=socket(AF_INET,SOCK_DGRAM,0);

   bzero(&servaddr,sizeof(servaddr));

   servaddr.sin_family = AF_INET;
   servaddr.sin_addr.s_addr=htonl(INADDR_ANY);
   servaddr.sin_port=htons(32001);
   bind(sockfd,(struct sockaddr *)&servaddr,sizeof(servaddr));


   while(1)
  {
	   
    len = sizeof(cliaddr);
    bzero(message, SIZE);
         
    n=recvfrom(sockfd,message,sizeof(message),0,(struct sockaddr *)&cliaddr,&len);  
   
    printf("%s\n",message); 
    fflush(stdout);

   
    system("sudo killall qemu-system-i386"); 
//    bzero(cmdbuf, 100);
  //  sprintf(cmdbuf, "cp -r /home/ubuntu/S2EDIR/build/qemu-release/s2e-last res/%s",message);
    //system(cmdbuf); 
    //string s(message);

 
    id++;
    //cout<<id<<endl;
   
    //int converted_number = htonl(result);
    //sendto(sockfd,&converted_number,sizeof(converted_number),0,(struct sockaddr *)&cliaddr,sizeof(cliaddr));
        
   }
}
