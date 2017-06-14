#compile commands for  scheduler used in S2E, libxml required 
CC=gcc
CFLAGS=-o
DEPS = event.h
scheduler: new_scheduler.c querying_interface.c EEFSM_checker.c tftp_rule1.c $(DEPS)
	$(CC) $(CFLAGS) scheduler new_scheduler.c querying_interface.c EEFSM_checker.c tftp_rule1.c -lxml2
