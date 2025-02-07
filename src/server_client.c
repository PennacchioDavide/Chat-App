#include "../include/server_client.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/select.h>

char server_message[BUFFER_SIZE] = "";
char client_message[BUFFER_SIZE] = "";
bool new_server_message = false;
bool new_client_message = false;
pthread_mutex_t message_mutex = PTHREAD_MUTEX_INITIALIZER;

struct socket_t *clients[MAX_CLIENTS];
int num_clients = 0;

void sendSocket(int sock, char *string) {
    if (send(sock, string, strlen(string), 0) < 0) {
        perror("Send failed");
    }
}

void broadcastMessage(const char *message, int sender_sock) {
    pthread_mutex_lock(&message_mutex);
    for (int i = 0; i < num_clients; i++) {
        if (clients[i] != NULL && clients[i]->sock != sender_sock) {
            if (send(clients[i]->sock, message, strlen(message), 0) < 0) {
                perror("Failed to send message");
            }
        }
    }
    pthread_mutex_unlock(&message_mutex);
}

struct socket_t *createServer() {
    struct socket_t *server = malloc(sizeof(struct socket_t));
    if (!server) {
        perror("Failed to allocate memory for server");
        exit(EXIT_FAILURE);
    }

    server->sock = socket(AF_INET, SOCK_STREAM, 0);
    if (server->sock < 0) {
        perror("Server socket creation failed");
        exit(EXIT_FAILURE);
    }

    server->address.sin_family = AF_INET;
    server->address.sin_addr.s_addr = INADDR_ANY;
    server->address.sin_port = htons(PORT);

    return server;
}

void runServer(struct socket_t *socket_serv) {
    int opt = 1;
    if (setsockopt(socket_serv->sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        perror("setsockopt failed");
        exit(EXIT_FAILURE);
    }

    if (bind(socket_serv->sock, (struct sockaddr *)&socket_serv->address, sizeof(socket_serv->address)) < 0) {
        perror("Failed to bind server");
        exit(EXIT_FAILURE);
    }

    if (listen(socket_serv->sock, MAX_CLIENTS) < 0) {
        perror("listen failed");
        exit(EXIT_FAILURE);
    }

    printf("Server is listening on port %d...\n", PORT);

    while (1) {
        struct sockaddr_in client_addr;
        socklen_t addrlen = sizeof(client_addr);
        int client_sock = accept(socket_serv->sock, (struct sockaddr *)&client_addr, &addrlen);
        if (client_sock < 0) {
            perror("accept failed");
            continue;
        }

        printf("New client connected...\n");

        pthread_mutex_lock(&message_mutex);
        if (num_clients < MAX_CLIENTS) {
            struct socket_t *client_socket = malloc(sizeof(struct socket_t));
            client_socket->sock = client_sock;
            client_socket->address = client_addr;

            clients[num_clients++] = client_socket;

            pthread_t client_thread_id;
            pthread_create(&client_thread_id, NULL, handleClient, client_socket);
        } else {
            printf("Max clients reached. Connection rejected.\n");
            close(client_sock);
        }
        pthread_mutex_unlock(&message_mutex);
    }

    close(socket_serv->sock);
}


void *handleClient(void *arg) {
    struct socket_t *socket_client = (struct socket_t *)arg;

    while (1) {
        int bytes_read = recv(socket_client->sock, socket_client->buffer, BUFFER_SIZE - 1, 0);
        if (bytes_read > 0) {
            socket_client->buffer[bytes_read] = '\0';  // Null-terminate the received message
            printf("Server received: %s\n", socket_client->buffer);

            // Broadcast the message to other clients
            broadcastMessage(socket_client->buffer, socket_client->sock);
        } else if (bytes_read == 0) {
            printf("Client disconnected.\n");
            break;
        } else {
            perror("Error receiving message");
            break;
        }
    }

    // Remove the client from the list
    pthread_mutex_lock(&message_mutex);
    for (int i = 0; i < num_clients; i++) {
        if (clients[i] == socket_client) {
            clients[i] = clients[num_clients - 1];
            num_clients--;
            break;
        }
    }
    pthread_mutex_unlock(&message_mutex);

    close(socket_client->sock);
    free(socket_client);
    return NULL;
}

struct socket_t *createClient() {
    struct socket_t *client = malloc(sizeof(struct socket_t));
    if (!client) {
        perror("Failed to allocate memory for client");
        exit(EXIT_FAILURE);
    }

    client->sock = socket(AF_INET, SOCK_STREAM, 0);
    if (client->sock < 0) {
        perror("Client socket creation failed");
        exit(EXIT_FAILURE);
    }

    client->address.sin_family = AF_INET;
    client->address.sin_port = htons(PORT);
    client->address.sin_addr.s_addr = inet_addr("127.0.0.1");

    return client;
}

void runClient(struct socket_t *socket_client) {
    while (connect(socket_client->sock, (struct sockaddr *)&socket_client->address, sizeof(socket_client->address)) < 0) {
        perror("Client connection failed, retrying...");
        sleep(1);
    }

    printf("Client connected to server...\n");

    fd_set read_fds;
    char recv_buffer[BUFFER_SIZE];
    while (1) {
        FD_ZERO(&read_fds);
        FD_SET(STDIN_FILENO, &read_fds);   // Watch standard input
        FD_SET(socket_client->sock, &read_fds);  // Watch socket

        int max_fd = socket_client->sock > STDIN_FILENO ? socket_client->sock : STDIN_FILENO;

        if (select(max_fd + 1, &read_fds, NULL, NULL, NULL) < 0) {
            perror("select error");
            break;
        }

        if (FD_ISSET(STDIN_FILENO, &read_fds)) {
            // Read message from the user
            char input_buffer[BUFFER_SIZE];
            if (fgets(input_buffer, sizeof(input_buffer), stdin) != NULL) {
                input_buffer[strcspn(input_buffer, "\n")] = 0;  // Remove newline
                if (strcmp(input_buffer, "quit") == 0) {
                    printf("Disconnecting...\n");
                    break;
                }

                // Send the message to the server
                sendSocket(socket_client->sock, input_buffer);
            }
        }

        if (FD_ISSET(socket_client->sock, &read_fds)) {
            // Receive message from the server
            int bytes_read = recv(socket_client->sock, recv_buffer, sizeof(recv_buffer) - 1, 0);
            if (bytes_read > 0) {
                recv_buffer[bytes_read] = '\0';
                printf("Message from server: %s\n", recv_buffer);
            } else if (bytes_read == 0) {
                printf("Server disconnected.\n");
                break;
            } else {
                perror("recv error");
                break;
            }
        }
    }

    close(socket_client->sock);
}