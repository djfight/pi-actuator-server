/**
 * This header file defines the message structure
 * used when communicating via TCP sockets.
 * @author Jarred Light <jlightd@gmail.com>
 */

#include <string>

#ifndef MESSAGE_H
#define MESSAGE_H

#define PACKET_SIZE 18

#define DEVICE_COMMAND 0
#define TERMINATE_COMMAND 1

struct device
{
    int pinNumber;
    bool signal;
};

struct message
{
    int from;
    int command; // can be DEVICE_COMMAND or TERMINATE_COMMAND
    struct device deviceStatus;
};



#endif
