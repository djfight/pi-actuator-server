/**
 * This header file defines the message structure
 * used when communicating through pipes.
 * @author Jarred Light <lightdj@etsu.edu>
 */
#ifndef MESSAGE_H
#define MESSAGE_H

struct message
{
    int from;
    char payload[32];
};

#endif