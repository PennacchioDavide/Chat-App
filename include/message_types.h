#ifndef MESSAGE_TYPES_H
#define MESSAGE_TYPES_H

#define MAX_MESSAGES 1000

typedef enum {
    CLIENT_MSG,
    SERVER_MSG
} MessageType;

typedef struct 
{
    char message[BUFFER_SIZE];
    MessageType type;
} Message;

#endif
