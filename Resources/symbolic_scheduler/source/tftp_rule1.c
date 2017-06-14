
#include "event.h"

void init_rule1 (struct Rule_EEFSM* rule){
	
	
	rule->id=1;            // now 8 events for tftp 
	rule->event[0].flag=1; // 1 represents event Data_Arrived_Receiver 
        rule->event[1].flag=2; // 2 represents event Ack_Arrived_Sender
	rule->event[2].flag=3; // 3 represents event Timeout_Sender
	rule->event[3].flag=4; // 4 represents event Timeout_Receiver
	rule->event[4].flag=5; // 5 represents event Data_Sent_Sender
	rule->event[5].flag=6; // 6 represents event Ack_Sent_Receiver
	rule->event[6].flag=7; // 7 represents event Sender_exit
        rule->event[7].flag=8; // 8 represents event Receiver_exit
        rule->event[8].flag=9; // 9 represents event receiver timer reset 
	

	rule->event[0].param=0; // initial value=0
	rule->event[1].param=0;
	rule->event[2].param=0; // initial value=0
	rule->event[3].param=0;
	rule->event[4].param=0; // initial value=0
	rule->event[5].param=0;
        rule->event[6].param=0;
        rule->event[7].param=0;
        rule->event[8].param=0;


        rule->machine_var[0]=BLOCK_LAST_rule1; // the last block number defined in the event.h 
	rule->machine_var[1]=0; // machine_var[1] represents the expected sequence number of data arrived in receiver    
	rule->machine_var[2]=0; // machine_var[2] represents the expected sequence number of Ack arrived in sender
	rule->machine_var[3]=0; // machine_var[3] represents the number of timeouts occurred in sender
	rule->machine_var[4]=0; // machine_var[4] represents the number of timeouts occurred in receiver
        rule->machine_var[5]=MAX_TIMEOUTS; // the relation is >=, donot need to add +1 as starting from zero, the Max timeouts defined in the event.h 
	rule->machine_var[6]=0;  // sender terminate status 
        rule->machine_var[7]=0;  // receiver terminate status 
        rule->machine_var[8]=0;  // Time wait status 
        rule->machine_var[9]=MAX_TIMEOUTS2;  // Server timeout
        
        rule->sizeOfvar=8; //1-8 for quering use 
	
	/*
	 State List for this rule:
	     state 0: Start state 
         State 1: Waiting for Events
         State 2: New Ack Packet sent
         State 3: New Data Packet sent 
         State 4: Sender Maximum Time-outs 
         state 5: Receiver Maximum Time-outs 
         state 6: Last Ack Packet sent(Receiver Time wait before termination )   
	     state 7: Last Ack Packet received 
	     state 8: Data Packet Retransmission
		 state 9: Ack Packet Retransmission
	     Accept: state id = 99
	     Error: 
     * 
     * For transitions to error state, just ignore them for no mating transition error 
	*/
	rule->current_state_id=0;     // initial state 
	rule->accept_state_id=99;
    bzero(rule->history_sequence,2000);
   // rule->history_sequence[0]=rule->current_state_id+'0';
   
     // state 0 in EEFSM
	rule->states[0].id=0;   
	//Event5:
	//transition 0
	rule->states[0].outgoing[0].event_flag=5;
	rule->states[0].outgoing[0].from_state_id=0;
	rule->states[0].outgoing[0].to_state_id=1;
	rule->states[0].outgoing[0].pred.predicate_left=&rule->current_event.param;
	rule->states[0].outgoing[0].pred.predicate_right=&rule->machine_var[1];  
        rule->states[0].outgoing[0].pred.predicate_extra=NULL;
	rule->states[0].outgoing[0].pred.predicate_relation=1; // == 
	rule->states[0].outgoing[0].assign.assignment_left=NULL;
	rule->states[0].outgoing[0].assign.assignment_right=NULL;
	
	
	
	
	// state 1 in EEFSM
	rule->states[1].id=1;   // id=0 is kept for use
	
	//Event1:
	//transition 1
	rule->states[1].outgoing[0].event_flag=1;
	rule->states[1].outgoing[0].from_state_id=1;
	rule->states[1].outgoing[0].to_state_id=1;
	rule->states[1].outgoing[0].pred.predicate_left=&rule->current_event.param;
	rule->states[1].outgoing[0].pred.predicate_right=&rule->machine_var[1];  
	rule->states[1].outgoing[0].pred.predicate_extra=&rule->machine_var[7];
	rule->states[1].outgoing[0].pred.predicate_extra2=&rule->machine_var[8];
        rule->states[1].outgoing[0].pred.predicate_relation=10; // extra ==1 || extra2==0 && ((left ==0 && !=right ) || (left <  right -1)) 
	rule->states[1].outgoing[0].assign.assignment_left=NULL;
	rule->states[1].outgoing[0].assign.assignment_right=NULL;
	
	//transition 2 
	rule->states[1].outgoing[1].event_flag=1;
	rule->states[1].outgoing[1].from_state_id=1;
	rule->states[1].outgoing[1].to_state_id=2;
	rule->states[1].outgoing[1].pred.predicate_left=&rule->current_event.param;
	rule->states[1].outgoing[1].pred.predicate_right=&rule->machine_var[1];
        rule->states[1].outgoing[1].pred.predicate_extra=&rule->machine_var[7];	
        rule->states[1].outgoing[1].pred.predicate_relation=11;//   extra==0 && x==v1 
	rule->states[1].outgoing[1].assign.assignment_left=&rule->machine_var[1];
	rule->states[1].outgoing[1].assign.assignment_right=&rule->machine_var[1];
	rule->states[1].outgoing[1].assign.assignment_relation=0; // left=right+1
	
           //transition 3
	rule->states[1].outgoing[2].event_flag=1;
	rule->states[1].outgoing[2].from_state_id=1;
	rule->states[1].outgoing[2].to_state_id=9;
	rule->states[1].outgoing[2].pred.predicate_left=&rule->current_event.param;
	rule->states[1].outgoing[2].pred.predicate_right=&rule->machine_var[1];
        rule->states[1].outgoing[2].pred.predicate_extra=&rule->machine_var[7];
        rule->states[1].outgoing[2].pred.predicate_extra2=&rule->machine_var[8];		
        rule->states[1].outgoing[2].pred.predicate_relation=14; //extra==0 && (left!=0 &&==right-1) || extra2==1  
	rule->states[1].outgoing[2].assign.assignment_left=NULL;
	rule->states[1].outgoing[2].assign.assignment_right=NULL;
	

	
	
	//Event2:
	
	//transition 4
	rule->states[1].outgoing[3].event_flag=2;
	rule->states[1].outgoing[3].from_state_id=1;
	rule->states[1].outgoing[3].to_state_id=1;
	rule->states[1].outgoing[3].pred.predicate_left=&rule->current_event.param;
	rule->states[1].outgoing[3].pred.predicate_right=&rule->machine_var[2];
        rule->states[1].outgoing[3].pred.predicate_extra=&rule->machine_var[6];	
        rule->states[1].outgoing[3].pred.predicate_relation=20; //extra=1 || <
	rule->states[1].outgoing[3].assign.assignment_left=NULL;
	rule->states[1].outgoing[3].assign.assignment_right=NULL;
	
	//transition 5
	rule->states[1].outgoing[4].event_flag=2;
	rule->states[1].outgoing[4].from_state_id=1;
	rule->states[1].outgoing[4].to_state_id=3;
	rule->states[1].outgoing[4].pred.predicate_left=&rule->current_event.param;
	rule->states[1].outgoing[4].pred.predicate_right=&rule->machine_var[2];
        rule->states[1].outgoing[4].pred.predicate_extra=&rule->machine_var[6];
        rule->states[1].outgoing[4].pred.predicate_relation=21;//   extra==0 && right=left && < last block number
	rule->states[1].outgoing[4].assign.assignment_left=&rule->machine_var[2];
	rule->states[1].outgoing[4].assign.assignment_right=&rule->machine_var[2];
	rule->states[1].outgoing[4].assign.assignment_relation=0; // left=right+1
	
	//transition 6
	rule->states[1].outgoing[5].event_flag=2;
	rule->states[1].outgoing[5].from_state_id=1;
	rule->states[1].outgoing[5].to_state_id=7;
	rule->states[1].outgoing[5].pred.predicate_left=&rule->current_event.param;
	rule->states[1].outgoing[5].pred.predicate_right=&rule->machine_var[2];  
	rule->states[1].outgoing[5].pred.predicate_extra=&rule->machine_var[6];
        rule->states[1].outgoing[5].pred.predicate_relation=22;//  extra==0 && right=left && == last block number
	rule->states[1].outgoing[5].assign.assignment_left=NULL;
	rule->states[1].outgoing[5].assign.assignment_right=NULL;
	
	//Event3:
	
	//transition 7
	rule->states[1].outgoing[6].event_flag=3;
	rule->states[1].outgoing[6].from_state_id=1;
	rule->states[1].outgoing[6].to_state_id=4;
	rule->states[1].outgoing[6].pred.predicate_left=&rule->machine_var[3];
	rule->states[1].outgoing[6].pred.predicate_right=&rule->machine_var[5];
    rule->states[1].outgoing[6].pred.predicate_extra=NULL;
    rule->states[1].outgoing[6].pred.predicate_relation=6; //>= max_timeouts
    rule->states[1].outgoing[6].assign.assignment_left=NULL;
	rule->states[1].outgoing[6].assign.assignment_right=NULL;
	

   
    //transition 8
	
	rule->states[1].outgoing[7].event_flag=3;
	rule->states[1].outgoing[7].from_state_id=1;
	rule->states[1].outgoing[7].to_state_id=8;
	rule->states[1].outgoing[7].pred.predicate_left=&rule->machine_var[3];
	rule->states[1].outgoing[7].pred.predicate_right=&rule->machine_var[5];
    rule->states[1].outgoing[7].pred.predicate_extra=NULL;	
    rule->states[1].outgoing[7].pred.predicate_relation=0; //left < right
	rule->states[1].outgoing[7].assign.assignment_left=&rule->machine_var[3];
	rule->states[1].outgoing[7].assign.assignment_right=&rule->machine_var[3];
	rule->states[1].outgoing[7].assign.assignment_relation=0;  // left = right +1 
	
	//Event4:
	//transition 9
	rule->states[1].outgoing[8].event_flag=4;
	rule->states[1].outgoing[8].from_state_id=1;
	rule->states[1].outgoing[8].to_state_id=5;
	rule->states[1].outgoing[8].pred.predicate_left=&rule->machine_var[4];
	rule->states[1].outgoing[8].pred.predicate_right=&rule->machine_var[9];
    rule->states[1].outgoing[8].pred.predicate_extra=&rule->machine_var[8];		
    rule->states[1].outgoing[8].pred.predicate_relation=18; //extra=1 || >= max_timeouts
	rule->states[1].outgoing[8].assign.assignment_left=NULL;
	rule->states[1].outgoing[8].assign.assignment_right=NULL;
	
	//transition 10
	rule->states[1].outgoing[9].event_flag=4;
	rule->states[1].outgoing[9].from_state_id=1;
	rule->states[1].outgoing[9].to_state_id=9;
	rule->states[1].outgoing[9].pred.predicate_left=&rule->machine_var[4];
	rule->states[1].outgoing[9].pred.predicate_right=&rule->machine_var[9];
        rule->states[1].outgoing[9].pred.predicate_extra=&rule->machine_var[8];
        rule->states[1].outgoing[9].pred.predicate_relation=19; //extra ==0 && < max_timeouts
	rule->states[1].outgoing[9].assign.assignment_left=&rule->machine_var[4];
	rule->states[1].outgoing[9].assign.assignment_right=&rule->machine_var[4];
	rule->states[1].outgoing[9].assign.assignment_relation=0; // left = right +1
	
	
        /*
	//Event1:
	//transition 32
	rule->states[1].outgoing[10].event_flag=1;

	rule->states[1].outgoing[10].from_state_id=1;
	rule->states[1].outgoing[10].to_state_id=5;
	rule->states[1].outgoing[10].pred.predicate_left=&rule->current_event.param;
	rule->states[1].outgoing[10].pred.predicate_right=&rule->machine_var[0];
        rule->states[1].outgoing[10].pred.predicate_extra=&rule->machine_var[8];		
        rule->states[1].outgoing[10].pred.predicate_relation=17; // extra ==1 && left ! = right 
	rule->states[1].outgoing[10].assign.assignment_left=NULL;
	rule->states[1].outgoing[10].assign.assignment_right=NULL;
        */
         //event 8: 
	//transition 33	
        rule->states[1].outgoing[10].event_flag=8;
	rule->states[1].outgoing[10].from_state_id=1;
	rule->states[1].outgoing[10].to_state_id=1; // 
	rule->states[1].outgoing[10].pred.predicate_left=NULL;
	rule->states[1].outgoing[10].pred.predicate_right=NULL; 
	rule->states[1].outgoing[10].pred.predicate_extra=&rule->machine_var[6]; 
        rule->states[1].outgoing[10].pred.predicate_relation=12;//extra == 0
	rule->states[1].outgoing[10].assign.assignment_left=&rule->machine_var[7];
	rule->states[1].outgoing[10].assign.assignment_right=NULL;
	rule->states[1].outgoing[10].assign.assignment_relation=2; // left = 1

	//transition 34
        rule->states[1].outgoing[11].event_flag=8;
	rule->states[1].outgoing[11].from_state_id=1;
	rule->states[1].outgoing[11].to_state_id=99; // to accept  
	rule->states[1].outgoing[11].pred.predicate_left=NULL;
	rule->states[1].outgoing[11].pred.predicate_right=NULL; 
	rule->states[1].outgoing[11].pred.predicate_extra=&rule->machine_var[6]; 
        rule->states[1].outgoing[11].pred.predicate_relation=13;//extra == 1
	rule->states[1].outgoing[11].assign.assignment_left=&rule->machine_var[7];
	rule->states[1].outgoing[11].assign.assignment_right=NULL;
	rule->states[1].outgoing[11].assign.assignment_relation=2; // left = 1


        //transition 35
        rule->states[1].outgoing[12].event_flag=9;
	rule->states[1].outgoing[12].from_state_id=1;
	rule->states[1].outgoing[12].to_state_id=1; // to accept  
	rule->states[1].outgoing[12].pred.predicate_left=&rule->current_event.param;
	rule->states[1].outgoing[12].pred.predicate_right=&rule->machine_var[4]; // par==2^(v4)) 
	rule->states[1].outgoing[12].pred.predicate_extra=NULL; 
        rule->states[1].outgoing[12].pred.predicate_relation=26;//par==2^(v4)) 
	rule->states[1].outgoing[12].assign.assignment_left=NULL;
	rule->states[1].outgoing[12].assign.assignment_right=NULL;
	



	// state 2 in EEFSM
	rule->states[2].id=2; 
	
	//Event6:
	
	//transition 11
	rule->states[2].outgoing[0].event_flag=6;
	rule->states[2].outgoing[0].from_state_id=2;
	rule->states[2].outgoing[0].to_state_id=1;
	rule->states[2].outgoing[0].pred.predicate_left=&rule->current_event.param;
	rule->states[2].outgoing[0].pred.predicate_right=&rule->machine_var[1]; 
	rule->states[2].outgoing[0].pred.predicate_extra=&rule->machine_var[7];
        rule->states[2].outgoing[0].pred.predicate_relation=4; // extra==0 && y==x-1 && < lAST_BLOCK
	rule->states[2].outgoing[0].assign.assignment_left=&rule->machine_var[4];
	rule->states[2].outgoing[0].assign.assignment_right=NULL;
	rule->states[2].outgoing[0].assign.assignment_relation=1; // left = 0
	
	//transition 12
	rule->states[2].outgoing[1].event_flag=6;
	rule->states[2].outgoing[1].from_state_id=2;
	rule->states[2].outgoing[1].to_state_id=6;
	rule->states[2].outgoing[1].pred.predicate_left=&rule->current_event.param;
	rule->states[2].outgoing[1].pred.predicate_right=&rule->machine_var[1];  
	rule->states[2].outgoing[1].pred.predicate_extra=&rule->machine_var[7];
    rule->states[2].outgoing[1].pred.predicate_relation=9; //extra==0 && y==x-1 && = lAST_BLOCK
	rule->states[2].outgoing[1].assign.assignment_left=&rule->machine_var[8];
	rule->states[2].outgoing[1].assign.assignment_right=NULL;
	rule->states[2].outgoing[1].assign.assignment_relation=2; // left = 1
	
	
	// state 3 in EEFSM
	
	rule->states[3].id=3; 

	//Event5:
	
	//transition 13
	rule->states[3].outgoing[0].event_flag=5;
	rule->states[3].outgoing[0].from_state_id=3;
	rule->states[3].outgoing[0].to_state_id=1;
	rule->states[3].outgoing[0].pred.predicate_left=&rule->current_event.param;
	rule->states[3].outgoing[0].pred.predicate_right=&rule->machine_var[2]; 
	rule->states[3].outgoing[0].pred.predicate_extra=&rule->machine_var[6];
    rule->states[3].outgoing[0].pred.predicate_relation=11; // (*pred.predicate_extra==0)&& left=right;
	rule->states[3].outgoing[0].assign.assignment_left=&rule->machine_var[3];
	rule->states[3].outgoing[0].assign.assignment_right=NULL;
	rule->states[3].outgoing[0].assign.assignment_relation=1; // left = 0
	
	
	// state 4 in EEFSM
	rule->states[4].id=4;  
    
	
	//Event 7:
	//transition 14
	
        rule->states[4].outgoing[0].event_flag=7;
	rule->states[4].outgoing[0].from_state_id=4;
	rule->states[4].outgoing[0].to_state_id=1; // 
	rule->states[4].outgoing[0].pred.predicate_left=&rule->machine_var[1];
	rule->states[4].outgoing[0].pred.predicate_right=NULL; 
	rule->states[4].outgoing[0].pred.predicate_extra=&rule->machine_var[7]; 
        rule->states[4].outgoing[0].pred.predicate_relation=25;//extra == 0 && left !=0 // server at least receive one packet 
	rule->states[4].outgoing[0].assign.assignment_left=&rule->machine_var[6];
	rule->states[4].outgoing[0].assign.assignment_right=NULL;
	rule->states[4].outgoing[0].assign.assignment_relation=2; // left = 1
    
	//Event 7:
	//transition 15
	
    rule->states[4].outgoing[1].event_flag=7;
	rule->states[4].outgoing[1].from_state_id=4;
	rule->states[4].outgoing[1].to_state_id=99; // to accept 
	rule->states[4].outgoing[1].pred.predicate_left=&rule->machine_var[1];
	rule->states[4].outgoing[1].pred.predicate_right=NULL; 
	rule->states[4].outgoing[1].pred.predicate_extra=&rule->machine_var[7]; 
    rule->states[4].outgoing[1].pred.predicate_relation=23;//extra == 1 || left =0, server does not receive any packet and generate any event.
	rule->states[4].outgoing[1].assign.assignment_left=&rule->machine_var[6];
	rule->states[4].outgoing[1].assign.assignment_right=NULL;
	rule->states[4].outgoing[1].assign.assignment_relation=2; // left = 1
	
	
    // state 5 in EEFSM
	rule->states[5].id=5;  
    
	
   //Event 8:
   //transition 16
    rule->states[5].outgoing[0].event_flag=8;
	rule->states[5].outgoing[0].from_state_id=5;
	rule->states[5].outgoing[0].to_state_id=1; // 
	rule->states[5].outgoing[0].pred.predicate_left=NULL;
	rule->states[5].outgoing[0].pred.predicate_right=NULL; 
	rule->states[5].outgoing[0].pred.predicate_extra=&rule->machine_var[6]; 
    rule->states[5].outgoing[0].pred.predicate_relation=12;//extra == 0
	rule->states[5].outgoing[0].assign.assignment_left=&rule->machine_var[7];
	rule->states[5].outgoing[0].assign.assignment_right=NULL;
	rule->states[5].outgoing[0].assign.assignment_relation=2; // left = 1

	//transition 17
    rule->states[5].outgoing[1].event_flag=8;
	rule->states[5].outgoing[1].from_state_id=5;
	rule->states[5].outgoing[1].to_state_id=99; // to accept  
	rule->states[5].outgoing[1].pred.predicate_left=NULL;
	rule->states[5].outgoing[1].pred.predicate_right=NULL; 
	rule->states[5].outgoing[1].pred.predicate_extra=&rule->machine_var[6]; 
    rule->states[5].outgoing[1].pred.predicate_relation=13;//extra == 1
	rule->states[5].outgoing[1].assign.assignment_left=&rule->machine_var[7];
	rule->states[5].outgoing[1].assign.assignment_right=NULL;
	rule->states[5].outgoing[1].assign.assignment_relation=2; // left = 1
	
	// state 6 Time wait in EEFSM 
	rule->states[6].id=6;   
      

    //Event 2:
	//transition 18
   	rule->states[6].outgoing[0].event_flag=2;
	rule->states[6].outgoing[0].from_state_id=6;
	rule->states[6].outgoing[0].to_state_id=7;
	rule->states[6].outgoing[0].pred.predicate_left=&rule->current_event.param;
	rule->states[6].outgoing[0].pred.predicate_right=&rule->machine_var[2];  
	rule->states[6].outgoing[0].pred.predicate_extra=&rule->machine_var[6];
    rule->states[6].outgoing[0].pred.predicate_relation=22;// extra=0&&  right=left && == last block number
	rule->states[6].outgoing[0].assign.assignment_left=NULL;
	rule->states[6].outgoing[0].assign.assignment_right=NULL;
    
	//transition 19
	rule->states[6].outgoing[1].event_flag=3;
	rule->states[6].outgoing[1].from_state_id=6;
	rule->states[6].outgoing[1].to_state_id=4;
	rule->states[6].outgoing[1].pred.predicate_left=&rule->machine_var[3];
	rule->states[6].outgoing[1].pred.predicate_right=&rule->machine_var[5];
    rule->states[6].outgoing[1].pred.predicate_extra=NULL;
    rule->states[6].outgoing[1].pred.predicate_relation=6; //>= max_timeouts
    rule->states[6].outgoing[1].assign.assignment_left=NULL;
	rule->states[6].outgoing[1].assign.assignment_right=NULL;
	
	//transition 20	
	rule->states[6].outgoing[2].event_flag=3;
	rule->states[6].outgoing[2].from_state_id=6;
	rule->states[6].outgoing[2].to_state_id=8;
	rule->states[6].outgoing[2].pred.predicate_left=&rule->machine_var[3];
	rule->states[6].outgoing[2].pred.predicate_right=&rule->machine_var[5];
    rule->states[6].outgoing[2].pred.predicate_extra=NULL;	
    rule->states[6].outgoing[2].pred.predicate_relation=0; //left < right
	rule->states[6].outgoing[2].assign.assignment_left=&rule->machine_var[3];
	rule->states[6].outgoing[2].assign.assignment_right=&rule->machine_var[3];
	rule->states[6].outgoing[2].assign.assignment_relation=0;  // left = right +1 
	
	//Event 4
	//transition 21	
	rule->states[6].outgoing[3].event_flag=4;
	rule->states[6].outgoing[3].from_state_id=6;
	rule->states[6].outgoing[3].to_state_id=5;
	rule->states[6].outgoing[3].pred.predicate_left=NULL;
	rule->states[6].outgoing[3].pred.predicate_right=NULL;
    rule->states[6].outgoing[3].pred.predicate_extra=NULL;	
    rule->states[6].outgoing[3].pred.predicate_relation=3; //always return true 
	rule->states[6].outgoing[3].assign.assignment_left=NULL;
	rule->states[6].outgoing[3].assign.assignment_right=NULL;

	//Event 8
	//transition 22	
    rule->states[6].outgoing[4].event_flag=8;
	rule->states[6].outgoing[4].from_state_id=6;	
	rule->states[6].outgoing[4].to_state_id=1; // 
	rule->states[6].outgoing[4].pred.predicate_left=NULL;
	rule->states[6].outgoing[4].pred.predicate_right=NULL; 
	rule->states[6].outgoing[4].pred.predicate_extra=&rule->machine_var[6]; 
    rule->states[6].outgoing[4].pred.predicate_relation=12;//extra == 0
	rule->states[6].outgoing[4].assign.assignment_left=&rule->machine_var[7];
	rule->states[6].outgoing[4].assign.assignment_right=NULL;
	rule->states[6].outgoing[4].assign.assignment_relation=2; // left = 1

	//transition 23
    rule->states[6].outgoing[5].event_flag=8;
	rule->states[6].outgoing[5].from_state_id=6;
	rule->states[6].outgoing[5].to_state_id=99; // to accept  
	rule->states[6].outgoing[5].pred.predicate_left=NULL;
	rule->states[6].outgoing[5].pred.predicate_right=NULL; 
	rule->states[6].outgoing[5].pred.predicate_extra=&rule->machine_var[6]; 
        rule->states[6].outgoing[5].pred.predicate_relation=13;//extra == 1
	rule->states[6].outgoing[5].assign.assignment_left=&rule->machine_var[7];
	rule->states[6].outgoing[5].assign.assignment_right=NULL;
	rule->states[6].outgoing[5].assign.assignment_relation=2; // left = 1
	 
        //Event 1:
	/*
         //transition 29 
   	rule->states[6].outgoing[6].event_flag=1;
	rule->states[6].outgoing[6].from_state_id=6;
	rule->states[6].outgoing[6].to_state_id=6;
	rule->states[6].outgoing[6].pred.predicate_left=&rule->current_event.param;
	rule->states[6].outgoing[6].pred.predicate_right=&rule->machine_var[0];  
	rule->states[6].outgoing[6].pred.predicate_extra=NULL;
        rule->states[6].outgoing[6].pred.predicate_relation=2;//   left!=right ;
	rule->states[6].outgoing[6].assign.assignment_left=NULL;
	rule->states[6].outgoing[6].assign.assignment_right=NULL;
        */

	//transition 30 
	rule->states[6].outgoing[6].event_flag=1;
	rule->states[6].outgoing[6].from_state_id=6;
	rule->states[6].outgoing[6].to_state_id=9;
	rule->states[6].outgoing[6].pred.predicate_left=&rule->current_event.param;
	rule->states[6].outgoing[6].pred.predicate_right=&rule->machine_var[0];
        rule->states[6].outgoing[6].pred.predicate_extra=NULL;	
        rule->states[6].outgoing[6].pred.predicate_relation=3;//   alywas return true  
	rule->states[6].outgoing[6].assign.assignment_left=NULL;
	rule->states[6].outgoing[6].assign.assignment_right=NULL;
	
        
       //transition 31 
	rule->states[6].outgoing[7].event_flag=2;
	rule->states[6].outgoing[7].from_state_id=6;
	rule->states[6].outgoing[7].to_state_id=1;
	rule->states[6].outgoing[7].pred.predicate_left=&rule->current_event.param;
	rule->states[6].outgoing[7].pred.predicate_right=&rule->machine_var[2];
        rule->states[6].outgoing[7].pred.predicate_extra=&rule->machine_var[6];	
        rule->states[6].outgoing[7].pred.predicate_relation=19; //<
	rule->states[6].outgoing[7].assign.assignment_left=NULL;
	rule->states[6].outgoing[7].assign.assignment_right=NULL;
  	
        
        //transition 36
        rule->states[6].outgoing[8].event_flag=9;
	rule->states[6].outgoing[8].from_state_id=6;
	rule->states[6].outgoing[8].to_state_id=6;   
	rule->states[6].outgoing[8].pred.predicate_left=&rule->current_event.param;
	rule->states[6].outgoing[8].pred.predicate_right=&rule->machine_var[4]; // par==2^(v4)) 
	rule->states[6].outgoing[8].pred.predicate_extra=NULL; 
        rule->states[6].outgoing[8].pred.predicate_relation=26;//par==2^(v4)) 
	rule->states[6].outgoing[8].assign.assignment_left=NULL;
	rule->states[6].outgoing[8].assign.assignment_right=NULL; 

//transition 37
        rule->states[6].outgoing[9].event_flag=2;
	rule->states[6].outgoing[9].from_state_id=6;
	rule->states[6].outgoing[9].to_state_id=6;   
	rule->states[6].outgoing[9].pred.predicate_left=NULL;
	rule->states[6].outgoing[9].pred.predicate_right=NULL;
	rule->states[6].outgoing[9].pred.predicate_extra=&rule->machine_var[6]; 
        rule->states[6].outgoing[9].pred.predicate_relation=13;//extra==1 
	rule->states[6].outgoing[9].assign.assignment_left=NULL;
	rule->states[6].outgoing[9].assign.assignment_right=NULL; 





	// state 7 in EEFSM
	rule->states[7].id=7;  
      
    //Event 7:
	//transition 24
    rule->states[7].outgoing[0].event_flag=7;
	rule->states[7].outgoing[0].from_state_id=7;
	rule->states[7].outgoing[0].to_state_id=6; // 
	rule->states[7].outgoing[0].pred.predicate_left=NULL;
	rule->states[7].outgoing[0].pred.predicate_right=NULL; 
	rule->states[7].outgoing[0].pred.predicate_extra=&rule->machine_var[7]; 
    rule->states[7].outgoing[0].pred.predicate_relation=12;//extra == 0
	rule->states[7].outgoing[0].assign.assignment_left=&rule->machine_var[6];
	rule->states[7].outgoing[0].assign.assignment_right=NULL;
	rule->states[7].outgoing[0].assign.assignment_relation=2; // left = 1
    
	//Event 7:
	//transition 25
    rule->states[7].outgoing[1].event_flag=7;
	rule->states[7].outgoing[1].from_state_id=7;
	rule->states[7].outgoing[1].to_state_id=99; // to accept 
	rule->states[7].outgoing[1].pred.predicate_left=NULL;
	rule->states[7].outgoing[1].pred.predicate_right=NULL; 
	rule->states[7].outgoing[1].pred.predicate_extra=&rule->machine_var[7]; 
    rule->states[7].outgoing[1].pred.predicate_relation=13;//extra == 1 
	rule->states[7].outgoing[1].assign.assignment_left=&rule->machine_var[6];
	rule->states[7].outgoing[1].assign.assignment_right=NULL;
	rule->states[7].outgoing[1].assign.assignment_relation=2; // left = 1
	
	//state 8 in EEFSM
	rule->states[8].id=8;  
      
    //Event 5:
	//transition 26
    rule->states[8].outgoing[0].event_flag=5;
	rule->states[8].outgoing[0].from_state_id=8;
	rule->states[8].outgoing[0].to_state_id=1; // to accept 
	rule->states[8].outgoing[0].pred.predicate_left=&rule->current_event.param;
	rule->states[8].outgoing[0].pred.predicate_right=&rule->machine_var[2]; 
	rule->states[8].outgoing[0].pred.predicate_extra=&rule->machine_var[6];
    rule->states[8].outgoing[0].pred.predicate_relation=11; // (*pred.predicate_extra==0)&& left=right;	
	rule->states[8].outgoing[0].assign.assignment_left=NULL;
	rule->states[8].outgoing[0].assign.assignment_right=NULL;

	//state 9 in EEFSM
	rule->states[9].id=9; 
	
	//transition 27
	rule->states[9].outgoing[0].event_flag=6;
	rule->states[9].outgoing[0].from_state_id=9;
	rule->states[9].outgoing[0].to_state_id=1;
	rule->states[9].outgoing[0].pred.predicate_left=&rule->current_event.param;
	rule->states[9].outgoing[0].pred.predicate_right=&rule->machine_var[1];  
	rule->states[9].outgoing[0].pred.predicate_extra=&rule->machine_var[7];
    rule->states[9].outgoing[0].pred.predicate_relation=4; //extra==0 && y==x-1 && < lAST_BLOCK
	rule->states[9].outgoing[0].assign.assignment_left=NULL;
	rule->states[9].outgoing[0].assign.assignment_right=NULL;
	
     //transition 28
	rule->states[9].outgoing[1].event_flag=6;
	rule->states[9].outgoing[1].from_state_id=9;
	rule->states[9].outgoing[1].to_state_id=6;
	rule->states[9].outgoing[1].pred.predicate_left=&rule->current_event.param;
	rule->states[9].outgoing[1].pred.predicate_right=&rule->machine_var[1];  
	rule->states[9].outgoing[1].pred.predicate_extra=&rule->machine_var[7];
    rule->states[9].outgoing[1].pred.predicate_relation=9; //extra==0 && y==x-1 && == lAST_BLOCK
	rule->states[9].outgoing[1].assign.assignment_left=NULL;
	rule->states[9].outgoing[1].assign.assignment_right=NULL;
	
	
   // Other no matching evens will transfer to error state. 
   
 //  printf("initialization for rule1 successfully\n");
  // printf("current_state_id: %d, accept state: %d \n", rule->current_state_id,rule->accept_state_id); 	
	}
	
