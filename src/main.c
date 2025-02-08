#include "../include/server_client.h"
#include "../include/thread.h"
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

int main(int argc, char *argv[])
{
    if (argc > 1 && strcmp(argv[1], "server") == 0)
    {
        struct socket_t *server = createServer();
        pthread_t server_tid;

        pthread_create(&server_tid, NULL, server_thread, server);
        pthread_join(server_tid, NULL);
    }
    else
    {
        if (argc < 3) {
            fprintf(stderr, "Usage: %s client <name>\n", argv[0]);
            return 1;
        }

        struct socket_t *client = createClient();
        
        // Create and populate the struct client_info
        struct client_info *info = malloc(sizeof(struct client_info));
        info->client = client;
        info->name = argv[2];  // Pass the client name

        pthread_t client_tid;

        // Pass the struct to the thread
        pthread_create(&client_tid, NULL, client_thread, info);
        pthread_join(client_tid, NULL);
    }

    return 0;
}
