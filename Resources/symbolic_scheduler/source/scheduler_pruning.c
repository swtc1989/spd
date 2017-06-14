#include <unistd.h>
#include <stdint.h>
#include <sys/shm.h>
#include "event.h"
#include <sys/signal.h>
#include <sys/types.h>
#include <sys/timeb.h>

#include "s2e.h"
//controller 
int ack_counter=0;
int data_counter=0;
int dup_N=1;
int controller_sockfd;  
struct sockaddr_in controller_servaddr;


// checker
#define DEBUG2 0


//global variable for use 
bool accept_flag=false;
struct event  event_list[N] ;
int init=1; //for symbolic initialization 
int id=2, ii, id_shmid=0, Last_retry=0;

uint32_t system_time=0;
uint16_t tmp_min;
int tmp_index, running;
int shmid, shmid2;
void *shmid_shm=NULL;  //changing to global variable for function atexit()
void *shmid_shm2=NULL;

struct shared_use_st* checking_prev_shm; 
int prev_client_send=0,prev_server_send=0 ;
int timer_map[10], timer_id; // At most 10 timers 
int isMin_index=0, finish, isDrop_index=0;
struct itimerval value;


struct Rule_EEFSM rule1;  
char warning_buf[1000];
char symbolic_name2[20];
char symbolic_name3[20];

// functions 
void warning_print(char* buf, char* message){

int tmp=strlen(buf);  
sprintf(buf+tmp,"%s",message);

return ; 

}

bool check_dup(int type, struct Rule_EEFSM* rule){


		int counter;
				
         switch(type){                 
             	case 1: 
		counter=rule->machine_var[3]; // machine_var[3] represents the number of time-outs occurred in sender
		return counter < dup_N ? true: false; 
	    
		case 2: 
		counter=rule->machine_var[4]; // machine_var[3] represents the number of time-outs occurred in sender
		return counter < dup_N ? true: false;
	   }

		  
}

void IsAccept(void){

       
       if (!final_check(&rule1))
       { 
        //warning_print(warning_buf,"*Notaccept");	

       } 
        else 
        {
        
        accept_flag=true;                  
        exit(0);
        }

                                           
}



void checker(struct event event_list[], int id, struct Rule_EEFSM* rule,int flag, int param){
	    struct Event_FSM triggered_event;   
        triggered_event.flag=flag; 
        triggered_event.param=param; 

        if (!update_check(rule, triggered_event, warning_buf)) // input the rule1 to be checked and the triggered event
	    {
		warning_print(warning_buf,"faults detected");
                s2e_warning(warning_buf);
                s2e_kill_state(0, "faults detected\n"); 	
       		exit(-1);
		}

      if (!query_to_controller(event_list, id, controller_sockfd, controller_servaddr, rule))
         {
         warning_print(warning_buf,"visited");
         s2e_warning(warning_buf);
         s2e_kill_state(0, "killed by the controller");
          
          exit(-1);
         }

		  
}


void checker_update_only(struct event event_list[], int id, struct Rule_EEFSM* rule,int flag, int param){
	    struct Event_FSM triggered_event;   
        triggered_event.flag=flag; 
        triggered_event.param=param; 

        if (!update_check(rule, triggered_event, warning_buf)) // input the rule1 to be checked and the triggered event
	    {
		    warning_print(warning_buf,"faults detected");
                    s2e_warning(warning_buf);
                    s2e_kill_state(0, "faults detected\n"); 	
       		exit(-1);
		}
		  
}



//


void symbolic_init(){
char symbolic_name[20];

init=0;
for (ii=2;ii<N;ii++)  // assume at most 100 events 
{
  event_list[ii].symbolic_mode=0;
  // for further use below 
  //event_relation[ii]=-1;
  sprintf(symbolic_name, "id %d", ii); 
  s2e_make_symbolic(&event_list[ii].symbolic_time,sizeof(event_list[ii].symbolic_time), symbolic_name);
//if (event_list[ii].symbolic_time>=65000)
 //{
//	s2e_kill_state(0, "kill > 2000 state ");			
 //} 

}
}



// to add final check for accept state and confirming to the controoler 
void delete_shared_memory(void ){

  if (accept_flag){
        warning_print(warning_buf,"*accept");
           
  }else{
        warning_print(warning_buf,"*not");
       
  }

        s2e_warning(warning_buf);   

        s2e_kill_state(0, "to finish\n");                              

}

void catchAlarm()
{ 
  running = 0;
  return;
}


int check_error(struct shared_use_st* shared){
 int i =0 ;  
  while (i < COSUMER_BUFSIZE )	
  {
   if (shared->shared_event[i].event_flag !=0) 
{
//printf("check error: event_flag %d, id %d \n ", shared->shared_event[i].event_flag,i );
return 0  ;
}      
    i++ ;
   }
 return 1 ;	
}

int accept_event(struct shared_use_st* shared){
	
     running = 1;//flag for running  
     signal(SIGALRM , catchAlarm); 
   //  alarm(TIMEOUT_DURATION); // the time to wait for coming event 
   if (init==0)  setitimer(ITIMER_REAL, &value, NULL); 
   
   while(running)//读取共享内存中的数据
	{
		//没有进程向共享内存定数据有数据可读取
		
		if(shared->produceCount-shared->consumerCount!= 0)
		{
		
//if (DEBUG2) printf("receving event %d: produceCOunt:%d, consumerCOunt:%d\n",shared->shared_event[shared->consumerCount%COSUMER_BUFSIZE].event_flag, shared->produceCount,shared->consumerCount );
		   

                   if (init==1) symbolic_init();
         		   setitimer(ITIMER_REAL, &value, NULL); 

		 
                   
                    switch (shared->shared_event[shared->consumerCount%COSUMER_BUFSIZE].event_flag) {
					case 8: //program_exit() receiver                            
                             checker_update_only(event_list, id, &rule1, 8, event_list[id].expected_seqnum);  
                             IsAccept();  							 
                             break;
							 
                    case 9: //program_exit() sender 
                             checker_update_only(event_list, id, &rule1, 7, event_list[id].expected_seqnum);
                             IsAccept();
                             break;
							 
                    //packt to server event 
                    case 10:              // not using event=event structure copy, need to copy eacn field member one by obe                                   			    

                             event_list[id].expected_seqnum=shared->shared_event[shared->consumerCount%COSUMER_BUFSIZE].expected_seqnum;//added 

                             if (data_counter< 0)
                           //  event_list[id].symbolic_time= 10; //0-2000
                             data_counter--;
                             
                             
                             event_list[id].event_time=event_list[id].symbolic_time+system_time;
                             event_list[id].event_flag=1;// change to fsm event flag
		                     event_list[id].src_id=shared->shared_event[shared->consumerCount%COSUMER_BUFSIZE].src_id;
		                     event_list[id].dst_id=shared->shared_event[shared->consumerCount%COSUMER_BUFSIZE].dst_id;
                             event_list[id].buffer_length=shared->shared_event[shared->consumerCount%COSUMER_BUFSIZE].buffer_length;
                             event_list[id].retry=shared->shared_event[shared->consumerCount%COSUMER_BUFSIZE].retry; // added 
		                     memcpy (event_list[id].packet_buffer,shared->shared_event[shared->consumerCount%COSUMER_BUFSIZE].packet_buffer,shared->shared_event[shared->consumerCount%COSUMER_BUFSIZE].buffer_length);// here it needs a deep copy 
                             prev_client_send=event_list[id].src_id;     
                             event_list[id].symbolic_mode=1;
                            //printf("10sequence: %d\n",event_list[id].expected_seqnum);
                            checker_update_only(event_list, id, &rule1, 5, event_list[id].expected_seqnum);

                            if (check_dup(1, &rule1)) event_list[id].symbolic_mode=1;
		            else event_list[id].symbolic_mode=0;
                            
                             id++;

                             break;
                    //packt to client event
                    case 11: 
                         
                             
                             event_list[id].event_flag=2;//// change to fsm event flag
		             event_list[id].src_id=shared->shared_event[shared->consumerCount%COSUMER_BUFSIZE].src_id;
		             event_list[id].dst_id=shared->shared_event[shared->consumerCount%COSUMER_BUFSIZE].dst_id;
                             event_list[id].buffer_length=shared->shared_event[shared->consumerCount%COSUMER_BUFSIZE].buffer_length;
                             
                             event_list[id].expected_seqnum=shared->shared_event[shared->consumerCount%COSUMER_BUFSIZE].expected_seqnum;//added 


                         //       if (ack_counter< 0)
                          //       event_list[id].symbolic_time= 10; //0-2000

                                   ack_counter--;
		             memcpy (event_list[id].packet_buffer,shared->shared_event[shared->consumerCount%COSUMER_BUFSIZE].packet_buffer,shared->shared_event[shared->consumerCount%COSUMER_BUFSIZE].buffer_length);// here it needs a deep copy 
                            

                             prev_server_send=event_list[id].src_id;
                          

                             event_list[id].event_time=event_list[id].symbolic_time+system_time;
                             event_list[id].retry=Last_retry; //ack donot have retry information from shared memory directly 
                          //    printf("11ss%dr%d,",event_list[id].expected_seqnum,event_list[id].retry);
                         //printf("11sequence: %d\n",event_list[id].expected_seqnum);  
                             checker_update_only(event_list, id, &rule1, 6, event_list[id].expected_seqnum);
                             if (check_dup(2, &rule1)) event_list[id].symbolic_mode=1;
			     else event_list[id].symbolic_mode=0;                                                         
                             id++; 
                             break;
                    //tigger timer in client_side
                    case -10: event_list[0].src_id = prev_client_send; //here it is tricky, as previous sendto and outgoing recvfrom should have the same port numberf 
                             //event_list[0].symbolic_time= TIMEOUT_DURATION_retransmit; // retransmit duration 
                             event_list[0].symbolic_time= shared->shared_event[shared->consumerCount%COSUMER_BUFSIZE].expected_seqnum;
                             event_list[0].event_time=event_list[0].symbolic_time+system_time;
                             event_list[0].symbolic_mode=1;
                             event_list[0].event_flag=3;// change to fsm event flag updating hook!!!
                           //  printf("-10");
                             break;
                    //reset timer in client_side
                    case -11: event_list[0].symbolic_mode=0;
                         //    printf("-11");
                             break;
                    //tigger timer in server_side
                    case -20: 
                         //    printf("-20");  
                             event_list[1].src_id = shared->shared_event[shared->consumerCount%COSUMER_BUFSIZE].src_id;
                             //event_list[1].symbolic_time=TIMEOUT_DURATION_retransmit ; //TIMEOUT_DURATION_retransmit; // retransmit duration
                             event_list[1].symbolic_time=shared->shared_event[shared->consumerCount%COSUMER_BUFSIZE].expected_seqnum; 
                             event_list[1].event_time=event_list[1].symbolic_time+system_time;
                             event_list[1].symbolic_mode=1;
                             event_list[1].event_flag=4;// change to fsm event flag
checker_update_only(event_list, 1, &rule1, 9, event_list[1].symbolic_time/1000);
                        
                             break;
                   //reset timer in server_side
                    case -21: 
                          //   printf("-21");
                             event_list[1].symbolic_mode=0; 
                             break; 
                    }         

                   shared->shared_event[shared->consumerCount%COSUMER_BUFSIZE].event_flag=0;//all resetting to 0                 		   			
                   shared->consumerCount++;
                   checking_prev_shm = shared ; 
		}

	}
	
	return 0;
	
}
int triger_timer(struct event tmp){
	
    int running = 1;  
    void *shm = NULL;  
    struct shared_use_st *shared = NULL;  
    int shmid;  
  
    shmid = shmget((key_t)tmp.src_id, sizeof(struct shared_use_st), 0666|IPC_CREAT);  //triger the timer in source_id
   //if (DEBUG2) printf("triger timer event:from %d to %d, shmid: %d \n",tmp.src_id, tmp.dst_id,shmid);
      
    if(shmid == -1)  
    {  
        s2e_kill_state(0, "shmget failed\n");
        return -1;  
    } 
    //attach
    shm = shmat(shmid, (void*)0, 0);  
    
	if(shm == (void*)-1)  
    {
        s2e_kill_state(0, "shmget failed\n");  
        return -1;  
    }  
    //if (DEBUG) printf("Memory attached at %X\n", (int)shm);  
    shared = (struct shared_use_st*)shm;  
      
 	
	while(shared->produceCount-shared->consumerCount== COSUMER_BUFSIZE)  //default buffer size is 1  
    {  
           
    } 
       
        
    shared->shared_event[shared->produceCount%COSUMER_BUFSIZE]=tmp;

               //printf("copy buffer: %s\n", tmp.packet_buffer);
		
    shared->produceCount++;  

      
    //de-attach 

    if(shmdt(shm) == -1)  
    {  
       s2e_kill_state(0, "shmget failed\n");
        exit(EXIT_FAILURE);  
    } 
	
    //sleep(2);  
    return 0;  
}

int copy_packet(struct event tmp){
	
    int running = 1;  
    void *shm = NULL;  
    struct shared_use_st *shared = NULL;  
    int shmid;  
    // 
   // if (DEBUG) printf("shared_id:%d \n", tmp.shared_id); //event timer
    //if (DEBUG) printf("shared_buffer:%s \n", tmp.packet_buffer); //event timer
    
   // fflush(stdout); 
    shmid = shmget((key_t)tmp.dst_id, sizeof(struct shared_use_st), 0666|IPC_CREAT);  
   //  if (DEBUG2) printf("coping event:from %d to %d, shmid: %d \n",tmp.src_id, tmp.dst_id,shmid);
      
   if(shmid == -1)  
    {  
       s2e_kill_state(0, "shmget failed\n"); 
        return -1;  
    } 
    //attach
    shm = shmat(shmid, (void*)0, 0);  
    if(shm == (void*)-1)  
    {  
        s2e_kill_state(0, "shmget failed\n"); 
        return -1;  
    }  
    //if (DEBUG) printf("Memory attached at %X\n", (int)shm);  
    shared = (struct shared_use_st*)shm;  
      
 	
    while(shared->produceCount-shared->consumerCount== COSUMER_BUFSIZE)  //default buffer size is 1  
    {  
           
    } 
       
        
	        //memset(shared->shared_event[shared->produceCount%COSUMER_BUFSIZE].packet_buffer,0,BUFSIZE);
                //shared->shared_event[shared->produceCount%COSUMER_BUFSIZE].buffer_length = tmp.buffer_length; // donot forget it
                //shared->shared_event[shared->produceCount%COSUMER_BUFSIZE].src_id = tmp.src_id;
		//memcpy (shared->shared_event[shared->produceCount%COSUMER_BUFSIZE].packet_buffer,tmp.packet_buffer,tmp.buffer_length);

                shared->shared_event[shared->produceCount%COSUMER_BUFSIZE]=tmp;

               //printf("copy buffer: %s\n", tmp.packet_buffer);
		
               shared->produceCount++;  

      
    //de-attach 

    if(shmdt(shm) == -1)  
    {  
        s2e_kill_state(0, "shmget failed\n"); 
        exit(EXIT_FAILURE);  
    } 
	
    //sleep(2);  
    return 0;  
}


// global variable 




int main()
{
	
init_rule1(&rule1);// initialization for FSM rule1
bzero(&controller_servaddr,sizeof(controller_servaddr));// initialization for controller 
bzero(warning_buf, 1000);
init_controller(&controller_sockfd, &controller_servaddr); // initialization for the channel to the controller.


//initialization for events
//event_0 for client timer, event_1 for server timer
event_list[0].symbolic_mode=0;
event_list[1].symbolic_mode=0;
atexit(delete_shared_memory);


         value.it_value.tv_sec = 0;
         value.it_value.tv_usec = TIMEOUT_DURATION;//2 ms

	void *shm = NULL;//分配的共享内存的原始首地址
	struct shared_use_st *shared;//指向shm
	int i=0;           //共享内存标识符
	                                                //创建共享内存
	shmid = shmget((key_t)1234, sizeof(struct shared_use_st), 0666|IPC_CREAT);
	
	if(shmid == -1)
	{
		s2e_kill_state(0, "shmget failed\n");
		exit(EXIT_FAILURE);
	}
	//将共享内存连接到当前进程的地址空间
	shmid_shm = shmat(shmid, 0, 0);
	if(shmid_shm == (void*)-1)
	{
		s2e_kill_state(0, "shmget failed\n");
		exit(EXIT_FAILURE);
	}

	
	shared = (struct shared_use_st*)shmid_shm;
	shared->produceCount = 0;
	shared->consumerCount = 0;
  
           while (i < COSUMER_BUFSIZE )	
       {
            shared->shared_event[i].event_flag =0;
            i++; 

        }     
	// printf("check error222: event_flag %d, %d \n ", shared->shared_event[0].event_flag, shared->shared_event[1].event_flag );
	/// for shared memory with server
    void *shm2 = NULL;//分配的共享内存的原始首地址
	struct shared_use_st *shared2;//指向shm
	           //for server
	                                                //创建共享内存
	shmid2 = shmget((key_t)4321, sizeof(struct shared_use_st), 0666|IPC_CREAT);
	
	if(shmid2 == -1)
	{
		s2e_kill_state(0, "2shmget failed\n");
		exit(EXIT_FAILURE);
	}
	//将共享内存连接到当前进程的地址空间
	shmid_shm2 = shmat(shmid2, 0, 0);
	if(shmid_shm2 == (void*)-1)
	{
		s2e_kill_state(0, "2shmget failed\n");
		exit(EXIT_FAILURE);
	}

	
	shared2 = (struct shared_use_st*)shmid_shm2;
	shared2->produceCount = 0;
	shared2->consumerCount = 0;
        shared2->socket_index = 0;
	i=0;
        while (i < COSUMER_BUFSIZE )	
       {
            shared2->shared_event[i].event_flag =0;
            i++; 

        }  
//initialization
//if (DEBUG2) printf(" initialization successfull: shmid1: %d, shmid2: %d \n", shmid, shmid2);
        //wait for the first event from client. 
while ( id==2 ){
	
		 if (accept_event(shared)==-1) {
          	  // wait for event from server. 
                 s2e_kill_state(0, "accept failed\n");
	         exit (-1);
             }
	
	
}
   


///	
loop: 	
           //fin min 1
          //tmp_min=0;
          //tmp_index=-1;
          //int finish = 1 ;
         //s2e_enable_forking(); 
		  //for (ii=0;ii<=id;ii++)
          //{
		   //if (event_list[ii].symbolic_mode==1)
		   //{ 
	                  //finish =0; 
		     //if (tmp_index==-1)   // use symbolic variable as less as possible
			 //{
			 //tmp_min=event_list[ii].event_time;
			 //tmp_index=ii;
			 //}
			 //else
			//{
			//if (tmp_min > event_list[ii].event_time)
                        //{
                           //tmp_min=event_list[ii].event_time;
                           //// if (tmp_index==0) event_relation[ii]=1;
			   //tmp_index=ii;
                        //}
			//else
			//{
			   ////if (tmp_index==0) event_relation[ii]=0;	
					
			//}	
			
			//}

		   //}

          //}
             //     s2e_disable_forking(); 
		  // find min2
		  
		  tmp_min=0;
          tmp_index=-1;
          int tmp_i=0 ;
          finish = 1 ;
          
         
		  for (i=0;i<=id;i++)
          {
            if (event_list[i].symbolic_mode==1)
		      { 
	       	        finish =0; 
	       	   
		           uint8_t isCurrentIndexMin;
                   sprintf(symbolic_name2, "isMin_id%d", isMin_index++); 
                   s2e_make_symbolic(&isCurrentIndexMin,sizeof(isCurrentIndexMin), symbolic_name2);

                   if (isCurrentIndexMin > 100)
                   {
					   //current one is not min so skip for next variable 
                   }    
                   else{
                                      // consider current one as min

                   for(tmp_i=0; tmp_i<=id; tmp_i++)
	               {
		           if (event_list[tmp_i].symbolic_mode==1 && tmp_i !=i) // no need to compare with itself 
		           {

                           if (event_list[i].event_time >= event_list[tmp_i].event_time)
		           {
			     		 s2e_kill_state(0, "infeasible path");
	                   }
		           }
		         }
		           tmp_min=event_list[i].event_time;
		           tmp_index=i;
	               break;  
	              }
			   
                  }
		  }
		  
		  if ( !finish && tmp_index==-1) s2e_kill_state(0, "infeasible path");
		  
                  //if (DEBUG2) printf("index: %d, event_flag: %d \n", tmp_index, event_list[tmp_index].event_flag); 
                  
                      if (check_error(checking_prev_shm)==0) {
                     // check if missing any new events. 
                      s2e_kill_state(0, "checker failed\n");
	              goto exit; 
                  }     
                if (finish ==1 ) goto exit; 
                 
	    switch(event_list[tmp_index].event_flag)   // event flag = 0, to represent empty condition
       {
       case 3: //triger the timer in client_side

             checker(event_list, id, &rule1, 3, event_list[tmp_index].expected_seqnum);       
	      if (triger_timer(event_list[tmp_index])==-1) {
                  s2e_kill_state(0, "copy packet failed\n");
	          exit (-1);
                   }
	         system_time=event_list[tmp_index].event_time;
	         event_list[tmp_index].symbolic_mode=0;
             
	      if (accept_event(shared)==-1) {
          	 s2e_kill_state(0, "accept failed\n");   // wait for event from server. 
	         exit (-1);
             }	
		
                break;
       case 4: //triger the timer in server_side
	   
	      //  if (DEBUG) printf("-20"); //event timer
	      checker(event_list, id,&rule1, 4, event_list[tmp_index].expected_seqnum);
	      
		if (triger_timer(event_list[tmp_index])==-1) {
                      s2e_kill_state(0, "copy packet failed\n");	         
                           exit (-1);
                   }
	         system_time=event_list[tmp_index].event_time;
	         event_list[tmp_index].symbolic_mode=0;
             
	       if (accept_event(shared2)==-1) {
          	 s2e_kill_state(0, "accept failed\n");   // wait for event from server. 
	         exit (-1);
             }	
		break;	  
		

       case 1: //send packet to server 

                   //if (DEBUG)  printf("10"); //event timer
                 //  if (DEBUG) printf("seq%dretry%d,",event_list[tmp_index].expected_seqnum,event_list[tmp_index].retry);
                 
		//   if (DEBUG) fflush(stdout);

            
            //sprintf(symbolic_name3, "isDrop_id%d", isDrop_index++); 
           //int isDrop; 
           //s2e_make_symbolic(&isDrop,sizeof(isDrop), symbolic_name3);

		     
			 if (copy_packet(event_list[tmp_index])==-1) {
	          s2e_kill_state(0, "copy packet failed\n");
	          exit (-1);
             }
	         checker(event_list, id, &rule1, 1, event_list[tmp_index].expected_seqnum);
            
			 system_time=event_list[tmp_index].event_time;
			 
			 if (accept_event(shared2)==-1) {
          	 s2e_kill_state(0, "accept failed\n");   // wait for event from server. 
	         exit (-1);
             }
			 
			 event_list[tmp_index].symbolic_mode=0;
             
             break; //donot forget it;
		  
       case 2: // //send packet to client
	    //    if (DEBUG) printf("11"); //event timer
           //     if (DEBUG) printf("seq%dretry%d,",event_list[tmp_index].expected_seqnum,event_list[tmp_index].retry);
		//if (DEBUG) fflush(stdout); 
             
			 

			
	         if (copy_packet(event_list[tmp_index])==-1) {
	          s2e_kill_state(0, "copy packet failed\n");
                  exit (-1);
             }
             checker(event_list, id, &rule1, 2, event_list[tmp_index].expected_seqnum);
	        	        
			 system_time=event_list[tmp_index].event_time;
			 
			 if (accept_event(shared)==-1) {
          	 s2e_kill_state(0, "accept failed\n");  // wait for event from client. 
	         exit (-1);
             }
			 
				
	          event_list[tmp_index].symbolic_mode=0; 
				 
				

	        
	   
	   }
	   
           goto loop;  
	   

exit:	
   
            //delete shared memory 
	   //deattatch shared memory with current process 
           // call atexit() function
	return 0 ; 
}
