/**
 * Class: ServerSocket
 * This class represents a server connection class.
 * @author Jarred Light <jlightd@gmail.com>
 */
#ifndef SERVERSOCKET_CC
#define SERVERSOCKET_CC

#include <string>
#include <fstream>
#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <pthread.h>
#include <errno.h>
#include "message.h"
#include "socket.h"
#include "log.h"
#define STRLEN 81
using namespace std;
#endif

class ServerSocket : public Socket
{
   public:
      /**
        * This constructor accepts a port number for the socket to bind to.
        */
      ServerSocket(string portnum)
      {
         this->portnum = portnum.c_str();
      }

      /**
        * This function handles client communication to the server.
        */
      static void* handleRequest(void* arg)
      {
         pthread_detach(pthread_self());

         message msg;

         ServerSocket* serverSocket = ((ServerSocket*)arg);

         // Show the actual FILE descriptor used
         cout << "Server thread, connection = " << serverSocket->connection << endl;

         //read the setup file name from the client
         serverSocket->value = read(serverSocket->connection, (char*)&msg, sizeof(message));

         if (serverSocket->value < 0)
         {
            cout << "Error on recv" << endl;
            pthread_exit(0);
         }
         else if (serverSocket->value == 0)
         {
            cout << "End of transmission" << endl;
            pthread_exit(0);
         }

         cout << "Message received: " << msg.payload << endl;

         //setup the logfilename
         serverSocket->logger.setLogfileName(msg.payload);

         if(!serverSocket->logger.open())
         {
            cout << "An error occurred while trying to open the log file for writing." << endl;
            pthread_exit(0);
         }

            // Continue until the message is "quit"
         while ( strcmp(msg.payload, "Q") != 0 )
         {
            // Get the next message
            serverSocket->value = read(serverSocket->connection, (char*)&msg, sizeof(message));

            if (serverSocket->value < 0)
            {
               cout << "Error on recv" << endl;
               pthread_exit(0);
            }
            else if (serverSocket->value == 0)
            {
               cout << "End of transmission" << endl;
               pthread_exit(0);
            }

            cout << "Message received: " << msg.payload << endl;

         //write message to log
            serverSocket->logger.writeLogRecord(msg.payload);
         }

         if(!serverSocket->logger.close())
         {
            cout << "An error occurred while trying to close the log file." << endl;
            pthread_exit(0);
         }

         // Close the socket
         close(serverSocket->connection);
         pthread_exit(0);
      }

      /**
        * This function is an implementation from the parent socket class
        * to setup a socket connection for a server to listen.
        */
      virtual void setupSocketConnection()
	   {
         //create socket
         this->sockdesc = socket(AF_INET, SOCK_STREAM, 0);

         if(this->sockdesc < 0)
         {
            cout << "Error creating socket" << endl;
            exit(0);
         }

         //store addressing information
         if( getaddrinfo("0.0.0.0", this->portnum, NULL, &this->myinfo) != 0)
         {
            cout << "Error getting address" << endl;
            exit(0);
         }

         //bind the socket to the port specified
         if(bind(this->sockdesc, this->myinfo->ai_addr, this->myinfo->ai_addrlen) < 0)
         {
            cout << "Error binding to socket" << endl;
            exit(0);
         }

         cout << "Bind successful, using portnum = " << this->portnum << endl;

         //setup listener
         if(listen(this->sockdesc, 1) < 0)
         {
            cout << "Error in listen" << endl;
            exit(0);
         }

         cout << "Calling accept" << endl;

         this->connection = accept(this->sockdesc, NULL, NULL);//wait for client connection

         if(this->connection < 0)
         {
             cout << "Error in accept" << endl;
             exit(0);
         } 
         else
         {
            cout << "Calling pthread_create with connection = " << this->connection << endl;

            //spawn a thread
            pthread_create(&serverThread, NULL, &ServerSocket::handleRequest, (void*)this);
            cout << "After create" << endl;
            pthread_join(serverThread, NULL);
         }
      }

private:
   log logger;//used to log information
   pthread_t serverThread;//used to spawn a thread and handle logging
};
