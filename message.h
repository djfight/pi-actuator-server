/**
 * This header file defines the message structure
 * used when communicating via TCP sockets.
 * @author Jarred Light <jlightd@gmail.com>
 */

#ifndef MESSAGE_H
#define MESSAGE_H

// todo: redefine this struct to fit a contract for acting on GPIO pins.
struct message
{
    int from;
    char payload[32];
};

#endif