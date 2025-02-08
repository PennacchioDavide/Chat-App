#include "../include/thread.h"
#include <stdlib.h>

void *server_thread(void *arg) 
{
    struct socket_t *server = (struct socket_t *) arg;
    runServer(server);
    free(server);
    return NULL;
}

void *client_thread(void *arg) 
{
    struct client_info *info = (struct client_info *)arg;
    struct socket_t *client = info->client;
    char *name = info->name;

    runClient(client, name);  // Use the client and name from the structure
    free(client);
    free(info);  // Free the structure when done
    return NULL;
}