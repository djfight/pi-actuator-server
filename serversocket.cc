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
#include "connection.cc"
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
         this->retries = 0;
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
         if(getaddrinfo("0.0.0.0", this->portnum.c_str(), NULL, &this->myinfo) != 0)
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

         //setup listener to only listen to one connection at a time
         int maxConnections = 1;
         if(listen(this->sockdesc, maxConnections) < 0)
         {
            this->logger->writeErrorMessage("Error invoking listen()");
            exit(0);
         }
      }

      virtual void disconnect()
      {
         close(this->sockdesc);
      }

      void acceptNewConnection()
      {
         this->logger->writeInfoMessage("Calling accept()");

         //await a client to initiate a connection
         this->connection = accept(this->sockdesc, NULL, NULL);

         // if the accept call fails, then log it and return
         if(this->connection < 0)
         {
            if(this->retries > 5)
            {
               // if accept critically fails, then log the error
               perror("accept");
               this->logger->writeErrorMessage("Max retries exceeded. Status code:" + to_string(this->connection));
               exit(0);
            }

            this->logger->writeWarningMessage("Error when calling accept(). Status code:" + to_string(this->connection));
            this->retries++;
         }
         else
         {
            this->retries = 0;
            this->logger->writeInfoMessage("Calling pthread_create where connection number = " + to_string(this->connection));
            this->clientConnection = new Connection(this->connection, this->logger);

            // exit status of spawned thread
            int exitStatus;
            //spawn a thread
            pthread_create(&serverThread, NULL, &handleRequest, (void*)this->clientConnection);
            this->logger->writeInfoMessage("Successfully spawned new thread");
            pthread_join(serverThread, (void**)&exitStatus);

            this->logger->writeInfoMessage("Thread terminated with status code = " + to_string(exitStatus));
         }
      }

   private:
      log* logger;//used to log information
      pthread_t serverThread;//used to spawn a thread and handle logging
      int retries;
      Connection* clientConnection;

      static void* handleRequest(void* arg)
      {
         Connection* connection = ((Connection*)arg);

         int handleStatus = SUCCESSFUL_HANDLE;
         message message;

         connection->getLogger()->writeInfoMessage("Connection request, connection: " + to_string(connection->getSocketDescriptor()));

         while(handleStatus != TERMINATE_CONNECTION)
         {
            message = connection->readMessage();

            handleStatus = connection->handleMessage(message);
         }

         connection->closeConnection();
         pthread_exit(0);
      }
};
