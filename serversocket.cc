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
         this->retries = 0;
      }

      /**
        * This function handles client communication to the server.
        */
      static void* handleRequest(void* arg)
      {
         // Note: Why in the world was this ever in my old OS code? It doesn't work with the join paradigm?
         // pthread_detach(pthread_self());

         // extract ServerSocket from thread argument
         ServerSocket* serverSocket = ((ServerSocket*)arg);

         // log the file descriptor for the connection
         serverSocket->logger->writeInfoMessage("Server thread, connection = " + to_string(serverSocket->connection));

         char buffer[PACKET_SIZE];

         // await the first command
         serverSocket->value = read(serverSocket->connection, buffer, sizeof(buffer));

         if (serverSocket->value < 0)
         {
            serverSocket->logger->writeErrorMessage("Error on invoking recv()");
            close(serverSocket->connection);
            pthread_exit(0);
         }
         else if (serverSocket->value == 0)
         {
            serverSocket->logger->writeInfoMessage("Transmission terminated from client");
            close(serverSocket->connection);
            pthread_exit(0);
         }

         message receivedMessage = serverSocket->parseMessage(buffer);

         if (receivedMessage.command == DEVICE_COMMAND)
         {
            serverSocket->logger->writeInfoMessage("Received device command from " + to_string(receivedMessage.from));
            serverSocket->logger->writeInfoMessage("Setting pin #" + to_string(receivedMessage.deviceStatus.pinNumber) + " with signal of " + to_string(receivedMessage.deviceStatus.signal));
         }
         else if (receivedMessage.command == TERMINATE_COMMAND)
         {
            serverSocket->logger->writeInfoMessage("Received termination command from " + to_string(receivedMessage.from));
         }
         else
         {
            serverSocket->logger->writeWarningMessage("Received erroneous command from " + to_string(receivedMessage.from));
         }

         // Continue until receive of terminate command
         while (receivedMessage.command != TERMINATE_COMMAND)
         {
            // Get the next message
            serverSocket->value = read(serverSocket->connection, (char*)&receivedMessage, sizeof(message));

            if (serverSocket->value < 0)
            {
               serverSocket->logger->writeErrorMessage("Error on invoking recv()");
               close(serverSocket->connection);
               pthread_exit(0);
            }
            else if (serverSocket->value == 0)
            {
               serverSocket->logger->writeInfoMessage("Transmission terminated from client");
               int closeStatus = close(serverSocket->connection);
               serverSocket->logger->writeInfoMessage("Closing client connection: " + to_string(closeStatus));
               pthread_exit(0);
            }
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

            // exit status of spawned thread
            int exitStatus;
            //spawn a thread
            pthread_create(&serverThread, NULL, &ServerSocket::handleRequest, (void*)this);
            this->logger->writeInfoMessage("Successfully spawned new thread");
            pthread_join(serverThread, (void**)&exitStatus);

            this->logger->writeInfoMessage("Thread terminated with status code = " + to_string(exitStatus));
         }
      }

      message parseMessage(char buffer[])
      {
         // assupmtion: buffer will always be equal to the packet size
         string bufferString(buffer);

         // extract struct data from buffer
         int from = atoi(bufferString.substr(0, 9).c_str());
         int command = atoi(bufferString.substr(10, 11).c_str());
         int pinNumber = atoi(bufferString.substr(12, 14).c_str());
         bool signal = atoi(bufferString.substr(15, 16).c_str()) == 1;

         message m;
         m.from = from;
         m.command = command;

         device deviceStatus;
         deviceStatus.pinNumber = pinNumber;
         deviceStatus.signal = signal;

         m.deviceStatus = deviceStatus;

         return m;
      }

private:
   log* logger;//used to log information
   pthread_t serverThread;//used to spawn a thread and handle logging
   int retries;
};
