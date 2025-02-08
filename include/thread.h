#ifndef THREAD_H
#define THREAD_H

#include "server_client.h"

void *server_thread(void *arg);
void *client_thread(void *arg);

#endif
