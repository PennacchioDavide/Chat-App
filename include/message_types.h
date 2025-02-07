#ifndef MESSAGE_TYPES_H
#define MESSAGE_TYPES_H

#define MAX_MESSAGES 1000

typedef struct 
{
    char message[BUFFER_SIZE];
    int id;
} Message;

void addMessageToHistory(const char *message, int id);

#endif
