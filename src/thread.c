#include "../include/thread.h"
#include "../include/server_client.h"

void *server_thread(void *arg) {
    struct socket_t *server = createServer();
    runServer(server);
    free(server);
    return NULL;
}

void *client_thread(void *arg) {
    sleep(2);  // Give server time to start
    struct socket_t *client = createClient();

    printf("Client trying to connect to server...\n");

    // Try connecting until the connection is successful
    while (connect(client->sock, (struct sockaddr*)&client->address, sizeof(client->address)) < 0) {
        perror("Client connection failed, retrying...");
        sleep(1);  // Retry every 1 second
    }

    printf("Client connected to server...\n");

    while (1) {
        if (new_client_message) {
            sendSocket(client->sock, client_message); 
            new_client_message = false;
            printf("Client sent: %s\n", client_message);
        }

        int bytes_read = recv(client->sock, client->buffer, BUFFER_SIZE - 1, 0);
        if (bytes_read > 0) {
            client->buffer[bytes_read] = '\0';
            pthread_mutex_lock(&message_mutex);
            strcpy(server_message, client->buffer);
            new_server_message = true;
            pthread_mutex_unlock(&message_mutex);
            printf("Client received: %s\n", server_message);
        }
    }

    close(client->sock);
    free(client);
}


