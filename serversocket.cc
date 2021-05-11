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
using namespace std;
#endif

class ServerSocket : public Socket
{
   public:
      /**
        * This constructor accepts a port number for the socket to bind to.
        */
      ServerSocket(string portnum, log* logger)
      {
         this->portnum = portnum;
         this->logger = logger;
      }

      pthread_t getServerThread() {
         return this->serverThread;
      }

      /**
        * This function handles client communication to the server.
        */
      static void* handleRequest(void* arg)
      {
         // Note: Why in the world was this ever in my old OS code? It doesn't work with the join paradigm?
         // pthread_detach(pthread_self());

         message msg;

         ServerSocket* serverSocket = ((ServerSocket*)arg);

         // Show the actual FILE descriptor used
         serverSocket->logger->writeInfoMessage("Server thread, connection = " + to_string(serverSocket->connection));

         //read the setup file name from the client
         serverSocket->value = read(serverSocket->connection, (char*)&msg, sizeof(message));

         if (serverSocket->value < 0)
         {
            serverSocket->logger->writeErrorMessage("Error on invoking recv()");
            pthread_exit(0);
         }
         else if (serverSocket->value == 0)
         {
            serverSocket->logger->writeInfoMessage("Transmission terminated from client");
            pthread_exit(0);
         }

         serverSocket->logger->writeInfoMessage(msg.payload);

         // Continue until the message is "quit"
         while ( strcmp(msg.payload, "Q") != 0 )
         {
            // Get the next message
            serverSocket->value = read(serverSocket->connection, (char*)&msg, sizeof(message));

            if (serverSocket->value < 0)
            {
               serverSocket->logger->writeErrorMessage("Error on invoking recv()");
               pthread_exit(0);
            }
            else if (serverSocket->value == 0)
            {
               serverSocket->logger->writeInfoMessage("Transmission terminated from client");
               pthread_exit(0);
            }

            //write message to log
            serverSocket->logger->writeInfoMessage(msg.payload);
         }

         // Close the connection and gracefully exit
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
            this->logger->writeErrorMessage("Error creating socket");
            exit(0);
         }

         //store addressing information
         if( getaddrinfo("0.0.0.0", this->portnum.c_str(), NULL, &this->myinfo) != 0)
         {
            this->logger->writeErrorMessage("Error getting address");
            exit(0);
         }

         //bind the socket to the port specified
         if(bind(this->sockdesc, this->myinfo->ai_addr, this->myinfo->ai_addrlen) < 0)
         {
            this->logger->writeErrorMessage("Error binding to socket");
            exit(0);
         }

         this->logger->writeInfoMessage("Bind successful, using port " + this->portnum);

         //setup listener
         if(listen(this->sockdesc, 1) < 0)
         {
            this->logger->writeErrorMessage("Error invoking listen()");
            exit(0);
         }

         this->logger->writeInfoMessage("Calling accept()");


         // todo: abstract this into its own function to handle many accept calls
         //await a client to initiate a connection
         this->connection = accept(this->sockdesc, NULL, NULL);

         if(this->connection < 0)
         {
            this->logger->writeWarningMessage("Error when calling accept()");
             exit(0);
         }
         else
         {
            this->logger->writeInfoMessage("Calling pthread_create where connection number = " + to_string(this->connection));

            // exist status of spawned thread
            int exitStatus;
            //spawn a thread
            pthread_create(&serverThread, NULL, &ServerSocket::handleRequest, (void*)this);
            this->logger->writeInfoMessage("Successfully spawned new thread");
            pthread_join(serverThread, (void**)&exitStatus);

            this->logger->writeInfoMessage("Thread terminated with status code = " + to_string(exitStatus));
         }
      }

      virtual void disconnect()
      {
         // todo: cleanup socket
      }

private:
   log* logger;//used to log information
   pthread_t serverThread;//used to spawn a thread and handle logging
};
