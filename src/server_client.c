#include "../include/server_client.h"

void sendSocket(int sock, char *string) {
    if (send(sock, string, strlen(string), 0) < 0)
    {
        perror("Send failed");
    }
}

struct socket_t *createServer() {
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

    return socket_client;
}

void runServer(struct socket_t *socket_serv) {
    int opt = 1;
    if (setsockopt(socket_serv->sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
    {
        perror("setsockopt failed");
        exit(EXIT_FAILURE);
    }

    if (bind(socket_serv->sock, (struct sockaddr*)&socket_serv->address, sizeof(socket_serv->address)) < 0)
    {
        perror("Server bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(socket_serv->sock, 3) < 0)
    {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d...\n", PORT);

    int addrlen = sizeof(socket_serv->address);
    int new_sock = accept(socket_serv->sock, (struct sockaddr*)&socket_serv->address, (socklen_t*)&addrlen);
    if (new_sock < 0)
    {
        perror("Accept failed");
        exit(EXIT_FAILURE);
    }
    printf("Server accepted a connection\n");

    char *message_server = "Hello from server";
    sendSocket(new_sock, message_server);

    int bytes_read = recv(new_sock, socket_serv->buffer, BUFFER_SIZE - 1, 0);
    if (bytes_read < 0)
    {
        perror("Server recv failed");
    } else 
    {
        socket_serv->buffer[bytes_read] = '\0';
        printf("Server received: %s\n", socket_serv->buffer);
    }

    close(new_sock);
    close(socket_serv->sock);
}

void runClient(struct socket_t *socket_client) {
    if (inet_pton(AF_INET, "127.0.0.1", &socket_client->address.sin_addr) <= 0)
    {
        perror("Invalid address/Address not supported");
        exit(EXIT_FAILURE);
    }

    if (connect(socket_client->sock, (struct sockaddr*)&socket_client->address, sizeof(socket_client->address)) < 0)
    {
        perror("Client connection failed");
        exit(EXIT_FAILURE);
    }
    printf("Client connected to server\n");

    char *message_client = "Hello from client";
    sendSocket(socket_client->sock, message_client);

    int bytes_read = recv(socket_client->sock, socket_client->buffer, BUFFER_SIZE - 1, 0);
    if (bytes_read < 0)
    {
        perror("Client recv failed");
    } else
    {
        socket_client->buffer[bytes_read] = '\0';
        printf("Client received: %s\n", socket_client->buffer);
    }

    close(socket_client->sock);
}

/*

int main() 
{
    int choice;

    struct socket_t *server = createServer();
    struct socket_t *client = createClient();

    printf("Choose mode:\n1. Server\n2. Client\n");
    scanf("%d", &choice);

    if (choice == 1) 
    {
        runServer(server);
    } else if (choice == 2) 
    {
        runClient(client);
    } else 
    {
        printf("Invalid choice\n");
    }

    free(server);
    free(client);

    return 0;
}

*/