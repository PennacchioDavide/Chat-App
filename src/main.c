#include "../include/server_client.h"
#include "../include/thread.h"
#include "../include/message_types.h"
#include "../include/database.h"

Message message_history[MAX_MESSAGES];
int message_count = 0;

void addMessageToHistory(const char *message, int id) 
{
    if (message_count < MAX_MESSAGES) 
    {
        strncpy(message_history[message_count].message, message, BUFFER_SIZE);
        message_history[message_count].id = id;
        message_count++;
    }
}

int main(int argc, char *argv[]) 
{
    int is_server = 0;

    if (argc > 1 && strcmp(argv[1], "server") == 0) 
    {
        is_server = 1;
    }

    if (is_server) 
    {
        struct socket_t *server = createServer();

        struct server_thread_args *server_args = malloc(sizeof(struct server_thread_args));
        server_args->server = server;

        pthread_t server_tid;
        pthread_create(&server_tid, NULL, server_thread, server_args);
        pthread_join(server_tid, NULL);  // Wait for the server thread to finish
        return 0;
    }

    struct socket_t *server = createClient();

    struct client_thread_args *client_args = malloc(sizeof(struct client_thread_args));
    client_args->server = server;

    pthread_t client_tid;
    pthread_create(&client_tid, NULL, client_thread, client_args);

    char input_buffer[256] = "";
    char name[256] = "";

    while (1) 
    {
        printf("%s: ", argv[1]);
        scanf("%s", input_buffer);
        if (strcmp(input_buffer, "") != 0) 
        {
            pthread_mutex_lock(&message_mutex);
            strcpy(client_message, input_buffer);
            new_client_message = true;
            addMessageToHistory(input_buffer, 1);
            strcpy(input_buffer, "");
            pthread_mutex_unlock(&message_mutex);
        }

        pthread_mutex_lock(&message_mutex);
        for (int i = 0; i < message_count; i++) 
        {
            if (message_history[i].id == 0) 
            {
                printf("Server: %s\n", message_history[i].message);
            }
        }
        pthread_mutex_unlock(&message_mutex);
    }

    pthread_join(client_tid, NULL);
    return 0;
}