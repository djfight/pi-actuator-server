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
#include <sstream>
#include <stdlib.h>
#include <string.h>
#include <cmath>
#include <iomanip>
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
    }

    /**
     * This function writes out a command to the pi actuation server
     * @param from
     * @param payload
     */
    void sendCommand(int command, int pinNumber, int signal)
	{
        message m;
        m.command = command;
        m.pinNumber = pinNumber;
        m.signal = signal;

        char buffer[PACKET_SIZE];

        string serializedMessage = this->serializeMessage(m);

        strncpy(buffer, serializedMessage.c_str(), sizeof(buffer));

        //send the message
        this->value = write(this->sockdesc, buffer, sizeof(buffer));
    }

    string serializeMessage(message m)
    {
        // assupmtion: buffer will always be equal to the packet size
        stringstream bufferStream;

        int pinNumberWidth = 2;
        int pinNumberSpaces = int(log10(m.pinNumber));

        // load buffer[0] to buffer[9] with "from" field
        bufferStream << m.command << " " << setw(pinNumberWidth - pinNumberSpaces) << m.pinNumber << " " << m.signal;

        string serializedMessage = bufferStream.str();

        bufferStream.clear();

        return serializedMessage;
    }


    virtual void disconnect()
    {
        close(this->sockdesc);
    }
	
	private:
	   const char* hostname;
	   const char* logfilename;
};
