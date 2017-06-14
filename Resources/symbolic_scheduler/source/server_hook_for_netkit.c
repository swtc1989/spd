#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/shm.h>
#include "event.h"
#define __USE_GNU
#include <dlfcn.h>
#include "s2e.h"
#include <netinet/in.h>
static int socket_index=0;
static int socket_fd[100]={0}; 
static int socket_fd_dest[100]={0}; 
static int prev_server_send=0; 

int close(int fd)  //offsetting the effects of original close function for "virtual" socketfd.   
{
     if( fd == -1)
{
   	 void *shm = NULL; 
      struct shared_use_st *shared; 
      int shmid;        
  
     shmid = shmget((key_t)4321, sizeof(struct shared_use_st), 0666|IPC_CREAT);  //1234 is the id to communicate with scheduler, client and server should be different 
	
	   if(shmid == -1)
	  {
		s2e_kill_state(0, "shmget failed\n");
		exit(EXIT_FAILURE);
	  }

	   shm = shmat(shmid, 0, 0);
	
	  if(shm == (void*)-1)
	 {
		s2e_kill_state(0, "shmget failed\n");
		exit(EXIT_FAILURE);
	 } 

	
	  shared = (struct shared_use_st*)shm;
	
     while(shared->produceCount-shared->consumerCount== COSUMER_BUFSIZE)  //default buffer size is 1  
    {  
           
    } 
       
   shared->shared_event[shared->produceCount%COSUMER_BUFSIZE].event_flag= 8;// 8 to terminate server; 
       
   shared->produceCount++;
   
       
    if(shmdt(shm) == -1)  
    {  
        s2e_kill_state(0, "shmget failed\n");
        exit(EXIT_FAILURE);  
    }  
}
 return 0;  //close(-1) to notify scheduler to terminate.    
}


unsigned int alarm(unsigned int seconds)
{ 
      void *shm = NULL; 
      struct shared_use_st *shared; 
      int shmid;        
  
  shmid = shmget((key_t)4321, sizeof(struct shared_use_st), 0666|IPC_CREAT);  //1234 is the id to communicate with scheduler, client and server should be different 
	
	   if(shmid == -1)
	  {
		s2e_kill_state(0, "shmget failed\n");
		exit(EXIT_FAILURE);
	  }

	   shm = shmat(shmid, 0, 0);
	
	  if(shm == (void*)-1)
	 {
		

		s2e_kill_state(0, "shmget failed\n");
		exit(EXIT_FAILURE);
	 } 

	
	  shared = (struct shared_use_st*)shm;
	
     while(shared->produceCount-shared->consumerCount== COSUMER_BUFSIZE)  //default buffer size is 1  
    {  
           
    } 
       
       if( seconds == 0)
       shared->shared_event[shared->produceCount%COSUMER_BUFSIZE].event_flag= -21;// -10: client triger timer. -11: client reset timer, -20: triger timer. -21: reset timer,10 : send packet to server, 11 send packet to client.
       else
       shared->shared_event[shared->produceCount%COSUMER_BUFSIZE].event_flag= -20;  
       
       shared->shared_event[shared->produceCount%COSUMER_BUFSIZE].expected_seqnum=seconds*1000; //the unit is second
       shared->shared_event[shared->produceCount%COSUMER_BUFSIZE].src_id=prev_server_send ;
       shared->produceCount++;
   
       
    if(shmdt(shm) == -1)  
    {  
       
	s2e_kill_state(0, "shmget failed\n");
        exit(EXIT_FAILURE);  
    }  
  

  return 0;

}

int socket (int __domain, int __type, int __protocol)
{
	//to-do: support TCP type, call original socket

          // printf("socket \n");
           //fflush(stdout);
           void *shm = NULL; 
           struct shared_use_st *shared; 
           int shmid;        
  
          shmid = shmget((key_t)4321, sizeof(struct shared_use_st), 0666|IPC_CREAT);  //1234 is the id to communicate with scheduler, client and server should be different 
	
	   if(shmid == -1)
	  {
		
		s2e_kill_state(0, "shmget failed\n");
		exit(EXIT_FAILURE);
	  }

	   shm = shmat(shmid, 0, 0);
	
	  if(shm == (void*)-1)
	 {
		
		s2e_kill_state(0, "shmget failed\n");
		exit(EXIT_FAILURE);
	 } 

	
	  shared = (struct shared_use_st*)shm;
          shared->socket_index++;
          socket_index= shared->socket_index;
         
         if(shmdt(shm) == -1)  
        {  
    
	s2e_kill_state(0, "shmget failed\n"); 
        exit(EXIT_FAILURE);  
        } 

	 socket_fd[socket_index]=socket_index+20000;
	
	return socket_index; 
}
//to do: close function 

int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen){
	const unsigned char *c;
        int port;      
        c=addr->sa_data;  // only support sa_family==AF_INET 
        port=256*c[0]+c[1];
	if (port !=0) socket_fd[sockfd]=port; 
   return 1;	
}

int connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen){
        int port;      
        const unsigned char *c;
        c=addr->sa_data;   
        port=256*c[0]+c[1];
	socket_fd_dest[sockfd]=port; 
   return 1;	

}

ssize_t send(int sockfd, const void *buf, size_t len, int flags) {
        void *shm = NULL;
	    struct shared_use_st *shared;
	    int shmid;          
	    const unsigned char *c;
        int port;        // only support sa_family==AF_INET 
        port=socket_fd_dest[sockfd];      

        const unsigned char *c2;
        c2=buf;
        int opcode;
        opcode=256* c2[0]+c2[1];
        int seq;

        if (opcode==2 ||opcode==1) seq=0; //read or write request 
        else seq=256*c2[2]+c2[3]; 

        
       shmid = shmget((key_t)4321, sizeof(struct shared_use_st), 0666|IPC_CREAT);  //1234 is the id to communicate with scheduler, client and server should be different 
	
	   if(shmid == -1)
	  {
	
		s2e_kill_state(0, "shmget failed\n");
		exit(EXIT_FAILURE);
	  }

	   shm = shmat(shmid, 0, 0);
	
	  if(shm == (void*)-1)
	 {
		
		s2e_kill_state(0, "shmget failed\n");
		exit(EXIT_FAILURE);
	 } 

	
	  shared = (struct shared_use_st*)shm;
	
     while(shared->produceCount-shared->consumerCount== COSUMER_BUFSIZE)  //default buffer size is 1  
    {  
           
    } 
       
        shared->shared_event[shared->produceCount%COSUMER_BUFSIZE].event_flag=11;// 10 : send packet to server, 11 send packet to client. 
        memset(shared->shared_event[shared->produceCount%COSUMER_BUFSIZE].packet_buffer,0,BUFSIZE); //clear up buffer 
        memcpy (shared->shared_event[shared->produceCount%COSUMER_BUFSIZE].packet_buffer,buf,len);// here it needs a deep copy 
        shared->shared_event[shared->produceCount%COSUMER_BUFSIZE].dst_id=port; // port number to identify shared_id
        shared->shared_event[shared->produceCount%COSUMER_BUFSIZE].src_id=socket_fd[sockfd];
        prev_server_send=socket_fd[sockfd]; // recording for server alarm
        shared->shared_event[shared->produceCount%COSUMER_BUFSIZE].buffer_length=len;
        shared->shared_event[shared->produceCount%COSUMER_BUFSIZE].expected_seqnum=seq; // adding to record of sequence number 
        //shared->shared_event[shared->produceCount%COSUMER_BUFSIZE].retry=flags; // adding to record of re-try 

        shared->produceCount++;
        
    if(shmdt(shm) == -1)  
    {  
        s2e_kill_state(0, "shmget failed\n"); 
        exit(EXIT_FAILURE);  
    }  


        return len;

}

ssize_t recv(int sockfd, void *buf, size_t len, int flags)  // only support flag = blocking, that is cannot support non-blocking   
{
  

        void *shm = NULL;
	 // if (DEBUG) printf("recvinside called\n");
     // if (DEBUG) fflush(stdout);	
	struct shared_use_st *shared;
	int shmid,length;        
	int key=socket_fd[sockfd]; 
	shmid = shmget((key_t)key, sizeof(struct shared_use_st), 0666|IPC_CREAT);
	
	if(shmid == -1)
	{
		
		s2e_kill_state(0, "shmget failed\n");
		exit(EXIT_FAILURE);
	}

	shm = shmat(shmid, 0, 0);
	if(shm == (void*)-1)
	{
	
		s2e_kill_state(0, "shmget failed\n");
		exit(EXIT_FAILURE);
	}

	shared = (struct shared_use_st*)shm;
       

    while(shared->produceCount-shared->consumerCount== 0)  //default buffer size is 1  
    {  
           
    }
  

    switch (shared->shared_event[shared->consumerCount%COSUMER_BUFSIZE].event_flag)  //here, triger timer ==== receive a packet with a event_flag of triger timer 
 {
 case 4:    //trigger timeout case // change to fsm flag
           errno = EINTR ; //change errno value for further re-transmissiton use; 
           length=-1; //erro return value
           break; 
 default:   //receiving packets case 
        length=shared->shared_event[shared->consumerCount%COSUMER_BUFSIZE].buffer_length;
        memcpy (buf, shared->shared_event[shared->consumerCount%COSUMER_BUFSIZE].packet_buffer,length);// here it needs a deep copy     
       
 } 	
        
    shared->consumerCount++;    
       
     if(shmdt(shm) == -1)
	{
		s2e_kill_state(0, "shmget failed\n");
		exit(EXIT_FAILURE);
	}
       
     //if (DEBUG) printf("return \n");
     //if (DEBUG) fflush(stdout);
     return length;
}

ssize_t recvfrom(int sockfd, void *buf, size_t len, int flags,
                 struct sockaddr *src_addr, socklen_t *addrlen)  // only support flag = blocking, that is cannot support non-blocking   
{
  

        void *shm = NULL;
       // printf("recvinside called\n");
        //fflush(stdout);	
	struct shared_use_st *shared;
	int shmid,length;        
	int key=socket_fd[sockfd]; 
	shmid = shmget((key_t)key, sizeof(struct shared_use_st), 0666|IPC_CREAT);
	
	if(shmid == -1)
	{
		
		s2e_kill_state(0, "shmget failed\n");
		exit(EXIT_FAILURE);
	}

	shm = shmat(shmid, 0, 0);
	if(shm == (void*)-1)
	{
	
		s2e_kill_state(0, "shmget failed\n");
		exit(EXIT_FAILURE);
	}

	shared = (struct shared_use_st*)shm;
       

    while(shared->produceCount-shared->consumerCount== 0)  //default buffer size is 1  
    {  
           
    }
  

    switch (shared->shared_event[shared->consumerCount%COSUMER_BUFSIZE].event_flag)  //here, triger timer ==== receive a packet with a event_flag of triger timer 
 {
 case 4:    //trigger timeout case // change to fsm flag
           errno = EINTR ; //change errno value for further re-transmissiton use; 
           length=-1; //erro return value
           break; 
 default:   //receiving packets case 
        length=shared->shared_event[shared->consumerCount%COSUMER_BUFSIZE].buffer_length;
        memcpy (buf, shared->shared_event[shared->consumerCount%COSUMER_BUFSIZE].packet_buffer,length);// here it needs a deep copy     
        if (src_addr !=NULL){
	       //struct sockaddr_in* servaddr=(struct sockaddr_in*) src_addr; // type conversion
               ((struct sockaddr_in*) src_addr)->sin_family = AF_INET;
               ((struct sockaddr_in*) src_addr)->sin_addr.s_addr=inet_addr("127.0.0.1");// here only consider local host communication 
               ((struct sockaddr_in*) src_addr)->sin_port=htons(shared->shared_event[shared->consumerCount%COSUMER_BUFSIZE].src_id);
	} 

 } 	
        
    shared->consumerCount++;    
       
     if(shmdt(shm) == -1)
	{
		s2e_kill_state(0, "shmget failed\n");
		exit(EXIT_FAILURE);
	}
       
   // printf("return \n");
    //fflush(stdout);
     return length;
}


