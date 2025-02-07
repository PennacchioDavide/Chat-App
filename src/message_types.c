#include "../include/message_types.h"
#include <string.h>

Message message_history[MAX_MESSAGES];
int message_count = 0;

void addMessageToHistory(const char *message, int id) {
    if (message_count < MAX_MESSAGES) {
        strncpy(message_history[message_count].message, message, BUFFER_SIZE);
        message_history[message_count].id = id;
        message_count++;
    }
}