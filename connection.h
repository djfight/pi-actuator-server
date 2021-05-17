#ifndef CONNECTION_H
#define CONNECTION_H

#include <stdlib.h>
#include <unistd.h>
#include "message.h"
#include "log.h"

#define TERMINATE_CONNECTION 1
#define SUCCESSFUL_HANDLE 2


class Connection
{
    public:
        Connection();
        Connection(int fileDescriptor, log* logger);
        void closeConnection();
        message readMessage();
        int handleMessage(message message);
        log* getLogger();
        int getSocketDescriptor();

    protected:
        int fileDescriptor;
        log* logger;

    private:
        message parseMessage(char buffer[]);
};

#endif