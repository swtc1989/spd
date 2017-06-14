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
int window=0, In_dup=0, Is_state=0, choice=0; 
bool outstanding_flag=false, history_flag=false;
int sockfd,n,id=0, result;
map<string, pair<int, map<string, string> > > namemap;  // xml string: pair<current_state, mapping of variables> ;
map<string, pair<int, map<string, string> > >::iterator it;
pair<map<string, string>::iterator, bool> res;

int current_state=0;

void print(void){

printf("Total visited counts: %d, and stop\n", id); 

exit(0);
}


/**
 * print_element_names:
 * @a_node: the initial xml node to consider.
 *
 * Prints the names of the all the xml elements
 * that are siblings or children of a given xml node.
 **/
 
void convert_only_state(string& str1){

string str2;
   
//cout<<"before:"<<str1<<endl; 
for (int i=0; i<str1.length(); i++){

if (str1[i]==':')
{

str2 +=str1[i-1];

}

}

str1=str2;
   
//cout<<"after:"<<str1<<endl;

return; 
}

void convert_window(string& str1, int window_size){

string str2;
//cout<<"before:"<<str1<<endl; 
//for (int i=str1.length()-window_size-1; i<str1.length()-1; i++){//the last one is current state if not update the eefsm
if (Is_state==1)
{
if (str1.length()<window_size) return;

for (int i=str1.length()-window_size; i<str1.length(); i++){
str2 +=str1[i];
}
} else{
 //2*8=16 bit for each configureation 
if (str1.length()<window_size*16) return;

for (int i=str1.length()-window_size*16; i<str1.length(); i++){
str2 +=str1[i];
}

}

str1=str2;
//cout<<"after:"<<str1<<endl;
return; 
}


static void print_element_names(xmlNode * a_node, map<string, string>& map)
{
    xmlNode *cur_node = NULL;

    for (cur_node = a_node; cur_node; cur_node = cur_node->next) {

      if (cur_node->type == XML_ELEMENT_NODE) {
        
     
      if (outstanding_flag && xmlHasProp (cur_node, BAD_CAST "type")) {
	    xmlChar *event_type = xmlGetProp (cur_node, BAD_CAST "type");
        xmlChar *event_para = xmlGetProp (cur_node, BAD_CAST "para");
        string tmp1((char*) event_type);
        //tmp1="e"+tmp1;
        string tmp2((char*) event_para);
        tmp2="s"+tmp2+"("+tmp1+")";       
        res=map.insert(pair<string, string>(tmp2, tmp1));
if (!In_dup){

	
while (!res.second){

        tmp2+=".1";       // to handle the duplicate retransmitted packet 
        res=map.insert(pair<string, string>(tmp2, tmp1));

}	
}
         } 
        else if (xmlHasProp (cur_node, BAD_CAST "number")) {
	    xmlChar *event_type = xmlGetProp (cur_node, BAD_CAST "number");
        xmlChar *event_para = xmlGetProp (cur_node, BAD_CAST "value");
        string tmp1((char*) event_type);
        tmp1="n"+tmp1;
        string tmp2((char*) event_para);
        tmp2="v"+tmp2;       
        map.insert(pair<string, string>(tmp1, tmp2));
       
		}
	else if (xmlHasProp (cur_node, BAD_CAST "current_state")) {
	xmlChar *event_type = xmlGetProp (cur_node, BAD_CAST "current_state");
        string tmp1="current_state:";
        string tmp2((char*) event_type);
        map.insert(pair<string, string>(tmp1, tmp2));

       } else if (history_flag && xmlHasProp (cur_node, BAD_CAST "state_transition")) {
	xmlChar *event_type = xmlGetProp (cur_node, BAD_CAST "state_transition"); //history information 
        string tmp1="state_transition:";
        string tmp2((char*) event_type);

        if (Is_state) convert_only_state(tmp2); 
        if (window>0) convert_window(tmp2, window); 


        map.insert(pair<string, string>(tmp1, tmp2));
       } 
}       print_element_names(cur_node->children, map);  
    }
}


void formMap(char* message, map<string, string>& map){

    xmlDocPtr doc; /* the resulting document tree */
    xmlNode *root_element = NULL;

    doc = xmlReadMemory(message, strlen(message), "noname.xml", NULL, XML_PARSE_NOBLANKS);
    
    if (doc == NULL) {
        fprintf(stderr, "Failed to parse %s\n", message);
	return;
    }
    /*Get the root element node */
    root_element = xmlDocGetRootElement(doc);

    print_element_names(root_element, map);

    /*free the document */
    xmlFreeDoc(doc);
    

}


bool diff_contain(map<string, string>& map1,map<string, string>& map2){
map<string, string>::iterator it2, result_it;

if (map1.size()>map2.size()) return false;

for (it2=map1.begin();it2!=map1.end(); ++it2){
  
result_it=map2.find(it2->first);

if (result_it == map2.end()) return false; //not found 

//cout<<"it2->first:"<<it2->first<<"it2->second:"<<it2->second<<"result->first:"<<result->first<<"result->seond:"<<result->second<<endl;
if (it2->second !=result_it->second) return false; // parameter !=

}

return true; 

}

int find_sub(map<string, string>& map){
	
	
	int result=0;


	for (it=namemap.begin(); it !=namemap.end(); ++it){
    if ( diff_contain(map,it->second.second)){ //subset 
//cout<<"current_state"<<current_state<<" history_state"<<it->second.first<<endl;
	if (current_state == it->second.first)
	result=2;
	else 
	return 1; 
	}
	
	}
	
	return result;
	
}

int find(map<string, string>& map){
	
	
	int result=0;


	for (it=namemap.begin(); it !=namemap.end(); ++it){ 
       if (map==it->second.second){
//cout<<"current_state"<<current_state<<" history_state"<<it->second.first<<endl;
	if (current_state == it->second.first)
	result=2;
	else 
	return 1; 
	}
	
	}
	
	return result;
	
}

int main(int argc, char**argv)
{
   signal(SIGINT , (void(*)(int)) print);
 if (argc==6 )  
    {
     window=atoi(argv[2]); 
     In_dup=atoi(argv[5]); 
     Is_state=atoi(argv[3]);
     history_flag=atoi(argv[1]);
     outstanding_flag=atoi(argv[4]);
    }
 else {
printf("usage: With History(0 or 1), Window_size,Is_state(0 or 1), With outstanding_packet(0 ir 1),Ingore_dup (0 or 1)\n");
 exit(0);
 } 
   struct sockaddr_in servaddr,cliaddr;
   socklen_t len;
   int states[SIZE]={0};
   char message[SIZE];
  // char state[10];

   sockfd=socket(AF_INET,SOCK_DGRAM,0);

   bzero(&servaddr,sizeof(servaddr));

   servaddr.sin_family = AF_INET;
   servaddr.sin_addr.s_addr=htonl(INADDR_ANY);
   servaddr.sin_port=htons(32000);
   bind(sockfd,(struct sockaddr *)&servaddr,sizeof(servaddr));


   while(1)
  {
	   
    len = sizeof(cliaddr);
    bzero(message, SIZE);
         
    n=recvfrom(sockfd,message,sizeof(message),0,(struct sockaddr *)&cliaddr,&len);  
   
   //printf("Response1 from server:%s\n",message); 
   
   
   
    
   
   if(strncmp(message,"state",5) == 0) 
   {  
  
            //printf("Total visited counts: %d, and stop\n", id); 
            current_state=atoi(message+5); 
            //printf("Current state: %d\n", current_state); 
         //   cout<<"Current state:"<<current_state<< endl;
          //  int converted_number = htonl(-1);
           // sendto(sockfd,&converted_number,sizeof(converted_number),0,(struct sockaddr *)&cliaddr,sizeof(cliaddr));  
            continue;
   }  
  
    string s(message);
    //cout<<namemap.size()<<endl;   
   if (namemap.count(s)>0)
    {
		  //cout << " is an element of mymap.\n";
		 if (current_state !=namemap.find(s)->second.first){
		  result=1; 
		 } else result=2 ;
    }
    else 
    {

	map<string, string> tmpmap;
        formMap(message, tmpmap);
/*
map<string, string> ::iterator i;
        for(i=tmpmap.begin();i!=tmpmap.end();i++)
    {
       cout<<"string1:"<<i->first<<"string2:"<<i->second<<endl;
    
       }

*/
        if (namemap.size()!=0)
       {
        choice= find(tmpmap);
        result=choice;
       }else result = 0;
 
        if (result==0) // not find a visited one
        {  
        id++;
        cout<<id<<endl;
        namemap.insert ( pair<string,pair<int, map<string, string> > > (s,pair<int, map<string, string> > (current_state, tmpmap) ) );
       } //else //cout << " is an element of mymap.\n";
    }
    
    int converted_number = htonl(result);
   sendto(sockfd,&converted_number,sizeof(converted_number),0,(struct sockaddr *)&cliaddr,sizeof(cliaddr));
        
   }
}
