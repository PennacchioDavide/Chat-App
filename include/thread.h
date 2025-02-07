#ifndef THREAD_H
#define THREAD_H

#include "server_client.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

struct server_thread_args {
    struct socket_t *server;
};

struct client_thread_args {
    struct socket_t *server;
};

void *server_thread(void *arg);
void *client_thread(void *arg);

#endif