/**
 * Author: Jarred Light
 * Date  : 05/09/2021
 * Purpose: connects to the pi-actuator-server for testing
*/
#ifndef TESTCLIENT
#define TESTCLIENT

// Include the C++ i/o library
#include <iostream>
// Include the C++ queue library
#include <queue>
// Include the C-string library
#include <string.h>
//bring in the string library
#include <string>
// Standard library
#include <stdlib.h>
//bring in the fstream library
#include <fstream>
//import wait libs
#include <sys/types.h>
#include <sys/wait.h>
//import vector library
#include <vector>
// Include the pthread library
#include <pthread.h>
// Include the semaphore library
#include <semaphore.h>
#include <unistd.h>
//bring in socket code
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <errno.h>
//bring in the log class
#include "message.h"
#include "log.h"
#include "clientsocket.cc"//client socket code for writing to the log server
// Shortcut for the std namespace
using namespace std;

// String length, plus one for \0
#define STRLEN 32
#define SOCKETINFOLEN 81
#endif

int main( int argc, char** argv )
{
    pthread_t* robots;//create an array of threads for the robots
    fstream setupFileReader;//setup a fstream to read the setup file
    fstream commandFileReader;//setup a fstream to read the command file
    string line;//used for reading in files
    message m;//setup a message struct for logging
    int numberOfRobots, maxX, maxY;//instantiate the max X and Y

    // getopt( ) requires char* arguments
    char hostname[STRLEN] = "", portnumber[STRLEN] = "", logfilename[STRLEN] = "";
    // For return value of getopt( )
    char c;
    // argc is the number of command-line arguments, always at least 1
    cout << "# command line arguments = " << argc << endl;

    // This depends on the specs - suppost that the command line
    // argument -s is required; if not present, quit
    if ( argc == 1 ) 
    {
        cout << "Usage: params -s setupfilename* -c cmdfilename -l logfilename" << endl;
        exit(0);
    }

    // Use getopt( ) to loop through -s, -c, -l flags
    while ( (c = getopt(argc, argv, "h:p:l:")) != -1 ) 
    {
        switch (c)
        {
            case 'h': strncpy(hostname, optarg, STRLEN-1);
                break;
            case 'p': strncpy(portnumber, optarg, STRLEN-1);
                break;
            case 'l': strncpy(logfilename, optarg, STRLEN-1);
                break;
        };
    }

    //client socket
    ClientSocket* clientSocket = new ClientSocket(hostname, portnumber, logfilename);

    clientSocket->setupSocketConnection();

    clientSocket->writeToLog(1, "Q");

    // close the socket connection after everything has finished up
    clientSocket->closeConnection();

    // Requires an integer return value
    return 0;
}