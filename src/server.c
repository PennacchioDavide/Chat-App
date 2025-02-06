#include "../include/server.h"

#define PORT 8080

struct server_socket *createServerSocket()
{
    struct server_socket *socket_serv = malloc(sizeof(struct server_socket));

    socket_serv->sock = socket(AF_INET, SOCK_STREAM, 0);

    if (socket_serv->sock < 0) {
        perror("Server socket creation failed");
    }

    socket_serv->address.sin_family = AF_INET;
    socket_serv->address.sin_addr.s_addr = INADDR_ANY;
    socket_serv->address.sin_port = htons(PORT);

    return socket_serv;
}

void bindReadServerSocket(struct server_socket *socket_serv)
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

void SendServerSocket(struct server_socket *socket_serv,  char *string)
{
    send(socket_serv->sock, string, strlen(string), 0);
}

int main()
{
    struct server_socket *socket_serv = createServerSocket();

    bindReadServerSocket(socket_serv);

    SendServerSocket(socket_serv, "hello");

    close(socket_serv->sock);

    return 0;
}