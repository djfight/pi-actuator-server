/**
 * Class: ClientSocket
 * This class represents a client connecting to a server and sending messages.
 * @author Jarred Light <jlightd@gmail.com>
 */

#ifndef CLIENTSOCKET_CC
#define CLIENTSOCKET_CC

#include <string>
#include <fstream>
#include <iostream>
#include <stdlib.h>
#include <string.h>
// Include the pthread library
#include <pthread.h>
//bring in socket code
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <errno.h>
#include "message.h"
#include "socket.h"
using namespace std;
#define STRLEN 32
#endif

class ClientSocket : public Socket
{
   public:
      ClientSocket(string hostname, string portnum, char logfilename[STRLEN])
	  {
         this->portnum = portnum.c_str();
         this->hostname = hostname.c_str();
         this->logfilename = logfilename;
      }

      virtual void setupSocketConnection()
	  {
        message msg;//holds message for logfilename
        this->sockdesc = socket(AF_INET, SOCK_STREAM, 0);//create socket
		
        if(this->sockdesc < 0)
		{
           cout << "Error creating socket" << endl;
           exit(0);
        }
		
        if(getaddrinfo(this->hostname, this->portnum.c_str(), NULL, &this->myinfo) != 0)
		{
		  //store address info
           cout << "Error getting address" << endl;
           exit(0);
        }
		
        //attempt connection to the host
        this->connection = connect(this->sockdesc, this->myinfo->ai_addr, this->myinfo->ai_addrlen);
        if(connection < 0)
		{
           cout << "Error in connect" << endl;
           exit(0);
        }

        cout << "Connection successful" << endl;
		  
        //copy the log file name into the buffer if the length is greater than zero otherwise use log.txt
        (strlen(logfilename) > 0) ?
			strncpy(msg.payload, this->logfilename, sizeof(msg.payload)) 
			: strncpy(msg.payload, "log.txt", sizeof(msg.payload));
		
		//send the message
        this->value = write(this->sockdesc, (char*)&msg, sizeof(message));
    }

    /**
     * // todo: refactor this to use a different struct
     * This function writes down a pipe to the logging process.
     * @param from
     * @param payload
     */
    void writeToLog(int from, string payload)
	{
        message m;
        m.from = from;
        strncpy(m.payload, payload.c_str(), sizeof(message));

        //send the message
        this->value = write(this->sockdesc, (char*)&m, sizeof(message));
    }

    void closeConnection()
	{
        close(this->sockdesc);
    }

    virtual void disconnect()
    {
        // todo: cleanup socket
    }
	
	private:
	   const char* hostname;
	   const char* logfilename;
};
