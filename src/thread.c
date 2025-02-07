#include "../include/thread.h"

void *server_thread(void *arg) {
    struct server_thread_args *args = (struct server_thread_args *)arg;
    runServer(args->server);
    free(args->server);
    free(args);
    return NULL;
}

void *client_thread(void *arg) {
    struct client_thread_args *args = (struct client_thread_args *)arg;
    runClient(args->server);
    free(args->server);
    free(args);
    return NULL;
}