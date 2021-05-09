#ifndef PIACTUATORSERVER
#define PIACTUATORSERVER

#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <iostream>
#include <string.h>
#include <stdio.h>
#include <pthread.h>
#include <errno.h>
#include <getopt.h>
#include "message.h"
#include "log.h"
#include "serversocket.cc"
using namespace std;
#define STRLEN 81

#endif

int main(int argc, char** argv )
{
   Socket* serverSocket;//create a server socket pointer
   char portnum[STRLEN];//create a portnumber array
   int optionIndex;//for return value of getopt()

   cout << "# command line arguments = " << argc << endl;

   if(argc == 1)
   {
      cout << "Usage: params -p portnum*" << endl;
      exit(0);
   }
   
   //get command params
   while((optionIndex = getopt(argc, argv, "p:")) != -1)
   {
      switch(optionIndex)
      {
         case 'p':
		 strncpy(portnum, optarg, STRLEN -1);
		 cout << "Port Number: " << portnum << endl;
		 break;
      }
      cout << "Still running..." << endl;
   }

   cout << "Port Number: " << portnum << endl;
   //create the server socket and setup the connection
   serverSocket = new ServerSocket(portnum);
   serverSocket->setupSocketConnection();

  return 0;
}

