/**
 * Class: Socket
 * This class is an abstract representation of setting up a socket connection.
 * @author Jarred Light <jlightd@gmail.com>
 */
#ifndef SERVER_H
#define SERVER_H
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <errno.h>
#include <string>
#include "message.h"

class Socket
{
	public:
	   Socket(){}
	   virtual void setupSocketConnection() = 0;
	   virtual void disconnect() = 0;

	protected:
	   struct addrinfo* myinfo;
	   int sockdesc;
	   std::string portnum;
	   int connection;
	   int value;
};
#endif
