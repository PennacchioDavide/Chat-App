#include "../include/server_client.h"

struct socket_t *createServer()
{
    struct socket_t *socket_serv = malloc(sizeof(struct socket_t));

    socket_serv->sock = socket(AF_INET, SOCK_STREAM, 0);

    if (socket_serv->sock < 0) {
        perror("Server socket creation failed");
    }

    socket_serv->address.sin_family = AF_INET;
    socket_serv->address.sin_addr.s_addr = INADDR_ANY;
    socket_serv->address.sin_port = htons(PORT);

    return socket_serv;
}

void runServer(struct socket_t *socket_serv)
{
    socklen_t addrlen = sizeof(socket_serv->address);
    int new_sock;

    if (bind(socket_serv->sock, (struct sockaddr*)&socket_serv->address, sizeof(socket_serv->address)) < 0)
    {
        perror("Server bind failed");
    }

    if (listen(socket_serv->sock, 3) < 0)
    {
        perror("listen");
    }

    if ((new_sock = accept(socket_serv->sock, (struct sockaddr*)&socket_serv->address, &addrlen)) < 0)
    {
        perror("accept");
    }

    read(new_sock, socket_serv->buffer, 1024 - 1);

    close(socket_serv->sock);
    socket_serv->sock = new_sock;
}

struct socket_t *createClient()
{
    struct socket_t *socket_client = malloc(sizeof(struct socket_t));

    socket_client->sock = socket(AF_INET, SOCK_STREAM, 0);

    if (socket_client->sock < 0) {
        perror("Server socket creation failed");
    }

    socket_client->address.sin_family = AF_INET;
    socket_client->address.sin_port = htons(PORT);

    return socket_client;
}

void runClient(struct socket_t *socket_client)
{
    if (inet_pton(AF_INET, "127.0.0.1", &socket_client->address.sin_addr)
        <= 0) {
        printf(
            "\nInvalid address/ Address not supported \n");
        return -1;
    }

    if ((connect(socket_client->sock, (struct sockaddr*)&socket_client->address, sizeof(socket_client->address))) < 0)
    {
        perror("Client Connection Failed");
    }

    read(socket_client->sock, socket_client->buffer, 1024 - 1);
}

void sendSocket(struct socket_t *socketT,  char *string)
{
    send(socketT->sock, string, strlen(string), 0);
}

int main() {
    int choice;

    struct socket_t *server = createServer();
    struct socket_t *client = createClient();

    printf("Choose mode:\n1. Server\n2. Client\n");
    scanf("%d", &choice);

    if (choice == 1) {
        runServer(server);
    } else if (choice == 2) {
        runClient(client);
    }

    sleep(10000);

    char *message_server = "hello by server";
    sendSocket(client->sock, message_server);

    char *message_client = "hello by client";
    sendSocket(client->sock, message_client);

    return 0;
}