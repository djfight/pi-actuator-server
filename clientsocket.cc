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
    void sendCommand(string from, int command, device deviceStatus)
	{
        message m;
        m.from = 1;
        m.command = command;
        m.deviceStatus = deviceStatus;

        string serializedMessage = this->serializeMessage(m);

        //send the message
        this->value = write(this->sockdesc, serializedMessage.c_str(), sizeof(serializedMessage));
    }

    string serializeMessage(message m)
    {
        // assupmtion: buffer will always be equal to the packet size
        stringstream bufferStream;

        // load buffer[0] to buffer[9] with "from" field
        int fromWidth = 10;
        int fromSpaces = int(log10(m.from));
        bufferStream << setw(fromWidth - fromSpaces) << m.from;

        // load buffer[10] to buffer[11] with "command" field
        bufferStream << " " << m.command;

        // load buffer[12] to buffer[14] with "pinNumber" field
        int pinNumberWidth = 2;
        int pinNumberSpaces = int(log10(m.deviceStatus.pinNumber));
        bufferStream << " " << setw(pinNumberWidth - pinNumberSpaces) << m.deviceStatus.pinNumber;

        // load buffer[15] to buffer[16] with "signal" field
        bufferStream << " " << m.deviceStatus.signal;

        return bufferStream.str();
    }


    virtual void disconnect()
    {
        close(this->sockdesc);
    }
	
	private:
	   const char* hostname;
	   const char* logfilename;
};
