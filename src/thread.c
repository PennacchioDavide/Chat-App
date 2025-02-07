#include "../include/thread.h"
#include "../include/server_client.h"
#include "../include/message_types.h"

void *server_thread(void *arg) 
{
    struct server_thread_args *args = (struct server_thread_args *)arg;
    struct socket_t *server = args->server;

    printf("Starting the server...\n");
    runServer(server);

    free(server);
    free(args);
    return NULL;
}


void *client_thread(void *arg) 
{
    struct client_thread_args *args = (struct client_thread_args *)arg;
    struct socket_t *server = args->server;

    struct socket_t *client = createClient();
    printf("Client trying to connect to the server...\n");

    while (connect(client->sock, (struct sockaddr *)&client->address, sizeof(client->address)) < 0) 
    {
        perror("Client connection failed, retrying...");
        sleep(1);
    }

    printf("Client connected to the server...\n");

    while (1) 
    {
        if (new_client_message) 
        {
            sendSocket(server->sock, client_message);
            new_client_message = false;
        }

        int bytes_read = recv(client->sock, client->buffer, BUFFER_SIZE - 1, 0);
        if (bytes_read > 0) 
        {
            client->buffer[bytes_read] = '\0';

            pthread_mutex_lock(&message_mutex);
            strcpy(server_message, client->buffer);
            addMessageToHistory(server_message, 0);
            new_server_message = true;
            pthread_mutex_unlock(&message_mutex);
        } 
        else if (bytes_read == 0) 
        {
            printf("Server disconnected.\n");
            break;
        } 
        else 
        {
            perror("Error receiving message");
        }
    }

    close(client->sock);
    free(client);
    free(args);
    return NULL;
}
