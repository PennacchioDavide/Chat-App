#include "../include/server_client.h"
#include "../include/thread.h"
#include "../include/message_types.h"

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    if (argc > 1 && strcmp(argv[1], "server") == 0) {
        struct socket_t *server = createServer();
        struct server_thread_args *server_args = malloc(sizeof(struct server_thread_args));
        server_args->server = server;

        pthread_t server_tid;
        pthread_create(&server_tid, NULL, server_thread, server_args);
        pthread_join(server_tid, NULL);
    } else {
        struct socket_t *client = createClient();
        struct client_thread_args *client_args = malloc(sizeof(struct client_thread_args));
        client_args->server = client;

        pthread_t client_tid;
        pthread_create(&client_tid, NULL, client_thread, client_args);

        char input[BUFFER_SIZE];
        while (1) {
            printf("You: ");
            fgets(input, BUFFER_SIZE, stdin);
            input[strcspn(input, "\n")] = '\0';

            if (strcmp(input, "quit") == 0) break;

            pthread_mutex_lock(&message_mutex);
            strncpy(client_message, input, BUFFER_SIZE);
            new_client_message = true;
            pthread_mutex_unlock(&message_mutex);
        }

        pthread_join(client_tid, NULL);
    }

    return 0;
}