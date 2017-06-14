#ifndef _SHMDATA_H_HEADER
#define _SHMDATA_H_HEADER

#define HEADER_SIZE 512
#define BUFSIZE 2048
#define COSUMER_BUFSIZE 20
#define TIMEOUT_DURATION 500000
#define TIMEOUT_DURATION_retransmit 5000 //5 s
#define N 100
#define DEBUG 1
#include <inttypes.h>
#include <string.h>
#include <sys/time.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>

typedef int bool;
#define true 1
#define false 0

struct event
{
	int event_flag;
	int symbolic_mode;
	int packet_flag;
	int retry;
	int expected_seqnum;
//int update_seqnum;
	uint32_t event_time;
	uint16_t symbolic_time;
	int buffer_length;
	int src_id;
	int dst_id;
	char header_buffer[HEADER_SIZE];
	char packet_buffer[BUFSIZE];
};

struct shared_use_st
{
	unsigned int blocknumber; //monitoring variables from source code for tftp
	unsigned int timeout_counter;

	unsigned int produceCount;//produceCount
	unsigned int consumerCount;
	unsigned int socket_index; //adding for server child process
	//unsigned int update_seqnum;
	struct event shared_event[COSUMER_BUFSIZE];// could change to [5] buffer size
};

// For FSM Checker use:
#define SIZE 20
#define BLOCK_LAST_rule1 3 // the first request block sequence is 0; 
#define MAX_TIMEOUTS 4 // the first one is 0; 0-4=5, for client timeouts
#define MAX_TIMEOUTS2 4 // for server timeouts

struct Event_FSM
{
	int flag;
	int param;
};

struct predicate
{

	int* predicate_left;
	int* predicate_right;
	int* predicate_extra;
	int* predicate_extra2;
	int predicate_relation;

};


struct assignment
{

	int* assignment_left;
	int* assignment_right;
	int assignment_relation;

};

struct Transition
{
	int from_state_id;
	int to_state_id;
	int event_flag;
	struct predicate pred;
	struct assignment assign;

};


struct State
{
	int id;
	struct Transition outgoing[SIZE];
};


struct Rule_EEFSM
{
	int id;
	struct State states[SIZE];
	struct Event_FSM event[SIZE];
	struct Event_FSM current_event;
	int current_state_id;
	int next_state_id;
	int accept_state_id;
	int machine_var[SIZE];
	int sizeOfvar;
	char history_sequence[2000]; //it may cause buffer overflow
};


#endif
