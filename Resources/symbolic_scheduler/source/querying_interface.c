#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "event.h"
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <sys/shm.h>
#include "s2e.h"



/*
struct current_monitoring_info
{
    int event_type;
    int state_num;
    char buf[1000]; // string message
    int vistited;

};
*/

void init_controller(int* sockfd, struct sockaddr_in* servaddr)
{


	*sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	servaddr->sin_family = AF_INET;
	servaddr->sin_addr.s_addr = inet_addr("10.0.2.2"); //change controller IP address
	servaddr->sin_port = htons(32000); // port number
	return;


}

void init_controller_terminate(int* sockfd, struct sockaddr_in* servaddr)
{


	*sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	servaddr->sin_family = AF_INET;
	servaddr->sin_addr.s_addr = inet_addr("10.0.2.2"); //change controller IP address
	servaddr->sin_port = htons(32001); // port number
	return;


}
void xml_process(char *buf, struct event event_list[], int id, struct Rule_EEFSM* rule)
{
	//   sprintf(buf,"(%d,%d,%d,%d,%d)", rule->current_state_id, rule->machine_var[1],rule->machine_var[2],rule->machine_var[3], rule->machine_var[4]); //specially for rule1

	xmlNodePtr root;
	xmlDocPtr doc;
	xmlChar *xmlbuff;
	int buffersize, i, ii;
	char tmp[10];
	bzero(tmp, 10);

	int size_node = 1;
	doc = xmlNewDoc(BAD_CAST "1.0");
	root = xmlNewNode(NULL, BAD_CAST "Global_state");
	xmlDocSetRootElement(doc, root);

	//current information

	xmlNodePtr node1 = xmlNewNode(NULL, BAD_CAST"FSM");
	size_node++;
	xmlAddChild(root, node1);
	sprintf(tmp, "%d", rule->current_state_id);
	xmlNewProp (node1, BAD_CAST "current_state", BAD_CAST tmp);

	for (i = 1; i <= rule->sizeOfvar; i++) // for macchine variables
	{
		xmlNodePtr node1_1 = xmlNewNode(NULL, BAD_CAST"Var");
		size_node++;

		bzero(tmp, 10);
		sprintf(tmp, "%d", i);
		xmlNewProp (node1_1, BAD_CAST "number", BAD_CAST tmp);

		bzero(tmp, 10);
		sprintf(tmp, "%d", rule->machine_var[i]);
		xmlNewProp (node1_1, BAD_CAST "value", BAD_CAST tmp);
		xmlAddChild(node1, node1_1);
	}

	// past information
	/*
	            xmlNodePtr node3 = xmlNewNode(NULL,BAD_CAST"History_sequence");
	            size_node++;

	            xmlNewProp (node3, BAD_CAST "state_transition", BAD_CAST rule->history_sequence);
	            xmlAddChild(root,node3);

	*/
	// Event list information only outstanding packets in the network
	xmlNodePtr node2 = xmlNewNode(NULL, BAD_CAST"Pending_event_list");
	size_node++;
	xmlAddChild(root, node2);
	int sizeOfevent = 0;

	for (ii = 2; ii <= id; ii++) // not including list[0],[1], which are two timers
	{

		if (event_list[ii].symbolic_mode == 1)
		{
			sizeOfevent++;
			xmlNodePtr node2_1 = xmlNewNode(NULL, BAD_CAST"Event");
			size_node++;
			bzero(tmp, 10);
			sprintf(tmp, "%d", event_list[ii].event_flag);
			xmlNewProp (node2_1, BAD_CAST "type", BAD_CAST tmp);

			bzero(tmp, 10);
			sprintf(tmp, "%d", event_list[ii].expected_seqnum);
			xmlNewProp (node2_1, BAD_CAST "para", BAD_CAST tmp);

			xmlAddChild(node2, node2_1);

		}

	}


	bzero(tmp, 10);
	sprintf(tmp, "%d", sizeOfevent);
	xmlNewProp (node2, BAD_CAST "size", BAD_CAST tmp);


	//final processing

	bzero(tmp, 10);
	sprintf(tmp, "%d", size_node);
	xmlNewProp (root, BAD_CAST "size", BAD_CAST tmp);
	xmlDocDumpFormatMemory(doc, &xmlbuff, &buffersize, 1);
	sprintf(buf, "%s", xmlbuff);
	xmlFree(xmlbuff);
	xmlFreeDoc(doc);
	return;
}

bool query_to_controller(struct event event_list[], int id, int sockfd, struct sockaddr_in servaddr,  struct Rule_EEFSM* rule)
{

	char buf[3000];
	bzero(buf, 3000);

	xml_process(buf, event_list, id, rule);

	sendto(sockfd, buf, sizeof(buf), 0,

	       (struct sockaddr *)&servaddr, sizeof(servaddr));

	int query_result = 0;

	int n = recvfrom(sockfd, &query_result, sizeof(query_result), 0, NULL, NULL);

	//if (n<0) s2e_kill_state(0,"udp channel to the controller error");

	int vistited = ntohl(query_result);
	// printf("receiving the result:%d\n", current_info.vistited);

	if (vistited == 1) // to kill
		return false;
	else  return true;

}


void query_terminate(bool flag, int id, int sockfd, struct sockaddr_in servaddr)
{


	char buf[8];
	bzero(buf, 8);
	sprintf(buf, "%d", id);

	if (!flag)
		sprintf(buf, "%derr", id);


	sendto(sockfd, buf, sizeof(buf), 0,

	       (struct sockaddr *)&servaddr, sizeof(servaddr));

	int query_result = 0;

	int n = recvfrom(sockfd, &query_result, sizeof(query_result), 0, NULL, NULL);

	s2e_kill_state(0, "found the bug terminated");

	// printf("receiving the result:%d\n", current_info.vistited);


}
