#ifndef MESSAGE_TYPES_H
#define MESSAGE_TYPES_H

#define MAX_MESSAGES 1000

#include <string.h>
#include "server_client.h"

typedef struct {
    char message[BUFFER_SIZE];
    int id; // 0 for server, >0 for clients
} Message;

void addMessageToHistory(const char *message, int id);

#endif