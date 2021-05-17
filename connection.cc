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

#ifndef CONNECTION_H
#include "connection.h"
#endif

Connection::Connection(int fileDescriptor, log* logger)
{
    this->fileDescriptor = fileDescriptor;
    this->logger = logger;
}

void Connection::closeConnection()
{
    int closeStatus = close(this->fileDescriptor);
    this->logger->writeInfoMessage("Closing client connection with status: " + to_string(closeStatus));
}

message Connection::readMessage()
{
    char buffer[PACKET_SIZE];
    message message;

    int readStatus = read(this->fileDescriptor, buffer, sizeof(buffer));

    // if there is a failure to read or the connection has been terminated, then set the appropriate message
    // todo: abstract this into a strategy pattern
    if(readStatus == 0)
    {
        this->logger->writeInfoMessage("Connection Terminated with status: " + to_string(readStatus));
        message.command = TERMINATE_COMMAND;

        return message;
    }
    else if(readStatus < 0)
    {
        this->logger->writeInfoMessage("Read call failed with status: " + to_string(readStatus));
        message.command = ERROR_COMMAND;

        return message;
    }

    message = this->parseMessage(buffer);

    return message;
}

message Connection::parseMessage(char buffer[])
{
    // assupmtion: buffer will always be equal to the packet size
    string bufferString(buffer);

    message m;

    if(bufferString.length() != PACKET_SIZE -1)
    {
        m.command = ERROR_COMMAND;
        
        return m;
    }

    // extract struct data from buffer
    int command = atoi(bufferString.substr(0, 0).c_str());
    int pinNumber = atoi(bufferString.substr(2, 3).c_str());
    bool signal = atoi(bufferString.substr(5, 5).c_str()) == 1;


    m.command = command;
    m.pinNumber = pinNumber;
    m.signal = signal;

    return m;
}

int Connection::handleMessage(message message)
{
    int status = TERMINATE_CONNECTION;

    if(message.command == DEVICE_COMMAND)
    {
        this->logger->writeInfoMessage("Received device command");
        this->logger->writeInfoMessage("Setting pin #" + to_string(message.pinNumber) + " with signal of " + to_string(message.signal));
        status = SUCCESSFUL_HANDLE;
    }
    else if(message.command == TERMINATE_COMMAND)
    {
        this->logger->writeInfoMessage("Received termination command");
    }
    else if(message.command == ERROR_COMMAND)
    {
        this->logger->writeInfoMessage("Received error during read");
    }
    else
    {
        this->logger->writeWarningMessage("Received erroneous command" + to_string(message.command));
    }

    return status;
}

log* Connection::getLogger()
{
    return this->logger;
}

int Connection::getSocketDescriptor()
{
    return this->fileDescriptor;
}