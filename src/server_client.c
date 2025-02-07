#include "../include/server_client.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 1024
#define MAX_CLIENTS 10

char server_message[BUFFER_SIZE] = "";
char client_message[BUFFER_SIZE] = "";
bool new_server_message = false;
bool new_client_message = false;
pthread_mutex_t message_mutex = PTHREAD_MUTEX_INITIALIZER;

struct socket_t *clients[MAX_CLIENTS];
int num_clients = 0;

void sendSocket(int sock, char *string) 
{
    if (send(sock, string, strlen(string), 0) < 0)
    {
        perror("Send failed");
    }
}

void broadcastMessage(const char *message, int sender_sock) 
{
    pthread_mutex_lock(&message_mutex);
    for (int i = 0; i < num_clients; i++) 
    {
        if (clients[i] != NULL && clients[i]->sock != sender_sock) 
        {
            sendSocket(clients[i]->sock, (char *)message);
        }
    }
    pthread_mutex_unlock(&message_mutex);
}

struct socket_t *createServer() 
{
    struct socket_t *socket_serv = malloc(sizeof(struct socket_t));
    if (socket_serv == NULL)
    {
        perror("Failed to allocate memory for server socket");
        exit(EXIT_FAILURE);
    }

    socket_serv->sock = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_serv->sock < 0) 
    {
        perror("Server socket creation failed");
        exit(EXIT_FAILURE);
    }

    socket_serv->address.sin_family = AF_INET;
    socket_serv->address.sin_addr.s_addr = INADDR_ANY;
    socket_serv->address.sin_port = htons(PORT);

    return socket_serv;
}

void runServer(struct socket_t *socket_serv) {
    int opt = 1;

    if (setsockopt(socket_serv->sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        perror("setsockopt échoué");
        exit(EXIT_FAILURE);
    }

    if (bind(socket_serv->sock, (struct sockaddr *)&socket_serv->address, sizeof(socket_serv->address)) < 0) {
        perror("Échec du bind du serveur");
        exit(EXIT_FAILURE);
    }

    if (listen(socket_serv->sock, 3) < 0) {
        perror("Échec du listen");
        exit(EXIT_FAILURE);
    }

    printf("Le serveur écoute sur le port %d...\n", PORT);

    int addrlen = sizeof(socket_serv->address);
    while (1) {
        // Accepter une nouvelle connexion client
        int new_sock = accept(socket_serv->sock, (struct sockaddr *)&socket_serv->address, (socklen_t *)&addrlen);
        if (new_sock < 0) {
            perror("Échec de l'accept");
            continue;
        }

        printf("Connexion acceptée par le serveur\n");

        // Ajouter le client à la liste des clients
        pthread_mutex_lock(&message_mutex);
        if (num_clients < MAX_CLIENTS) {
            struct socket_t *client_socket = malloc(sizeof(struct socket_t));
            client_socket->sock = new_sock;
            client_socket->address = socket_serv->address;
            clients[num_clients++] = client_socket;

            // Créer un thread pour gérer ce client
            pthread_t client_thread_id;
            pthread_create(&client_thread_id, NULL, handleClient, client_socket);
        } else {
            printf("Nombre maximum de clients atteint. Connexion refusée.\n");
            close(new_sock);
        }
        pthread_mutex_unlock(&message_mutex);
    }

    close(socket_serv->sock);
}


void *handleClient(void *arg) 
{
    struct socket_t *socket_client = (struct socket_t *)arg;

    while (1) 
    {
        int bytes_read = recv(socket_client->sock, socket_client->buffer, BUFFER_SIZE - 1, 0);
        if (bytes_read > 0) 
        {
            socket_client->buffer[bytes_read] = '\0';
            printf("[DEBUG] Server received: %s\n", socket_client->buffer); // Debugging server reception

            // Broadcast the message to all clients except the sender
            broadcastMessage(socket_client->buffer, socket_client->sock);
        } 
        else if (bytes_read == 0) 
        {
            printf("[DEBUG] Client disconnected.\n");
            break;
        } 
        else 
        {
            perror("[DEBUG] Error receiving message");
        }
    }

    // Handle client disconnection
    for (int i = 0; i < num_clients; i++) 
    {
        if (clients[i] == socket_client) 
        {
            clients[i] = clients[num_clients - 1];
            num_clients--;
            break;
        }
    }

    close(socket_client->sock);
    free(socket_client);
    return NULL;
}




struct socket_t *createClient() 
{
    struct socket_t *socket_client = malloc(sizeof(struct socket_t));
    if (socket_client == NULL) 
    {
        perror("Failed to allocate memory for client socket");
        exit(EXIT_FAILURE);
    }

    socket_client->sock = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_client->sock < 0) 
    {
        perror("Client socket creation failed");
        exit(EXIT_FAILURE);
    }

    socket_client->address.sin_family = AF_INET;
    socket_client->address.sin_port = htons(PORT);
    socket_client->address.sin_addr.s_addr = inet_addr("127.0.0.1");

    return socket_client;
}

void runClient(struct socket_t *socket_client) 
{
    while (connect(socket_client->sock, (struct sockaddr*)&socket_client->address, sizeof(socket_client->address)) < 0) 
    {
        perror("Client connection failed, retrying...");
        sleep(1); 
    }

    printf("Client connected to server...\n");

    while (1) {
        if (new_client_message) 
        {
            sendSocket(socket_client->sock, client_message); 
            new_client_message = false;
            printf("[DEBUG] Client sent: %s\n", client_message); // Debugging the sent message
        }

        int bytes_read = recv(socket_client->sock, socket_client->buffer, BUFFER_SIZE - 1, 0);
        if (bytes_read > 0) 
        {
            socket_client->buffer[bytes_read] = '\0';
            pthread_mutex_lock(&message_mutex);
            strcpy(server_message, socket_client->buffer);
            new_server_message = true;
            pthread_mutex_unlock(&message_mutex);

            // Debugging the received message
            printf("[DEBUG] Client received: %s\n", server_message);
        } 
        else if (bytes_read == 0) 
        {
            printf("[DEBUG] Server disconnected.\n");
            break;
        } 
        else 
        {
            perror("[DEBUG] Error receiving message");
        }
    }

    close(socket_client->sock);
    free(socket_client);

    close(socket_client->sock);
    free(socket_client);
}