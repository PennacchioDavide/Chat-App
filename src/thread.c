#include "../include/thread.h"
#include "../include/server_client.h"
#include "../include/message_types.h"

void *server_thread(void *arg) 
{
    // Création du serveur
    struct socket_t *server = createServer();

    // Lancement du serveur
    printf("Démarrage du serveur...\n");
    runServer(server);

    // Libération des ressources une fois le serveur arrêté
    free(server);
    return NULL;
}

void *client_thread(void *arg) 
{
    struct socket_t *client = createClient();

    printf("Client trying to connect to server...\n");

    while (connect(client->sock, (struct sockaddr*)&client->address, sizeof(client->address)) < 0) 
    {
        perror("Client connection failed, retrying...");
        sleep(1);
    }

    printf("Client connected to server...\n");

    while (1) 
    {
        // Envoi d'un message si le client en a un à envoyer
        if (new_client_message) 
        {
            sendSocket(client->sock, client_message); 
            new_client_message = false;
            printf("Client sent: %s\n", client_message);
        }

        // Réception d'un message du serveur
        int bytes_read = recv(client->sock, client->buffer, BUFFER_SIZE - 1, 0);
        if (bytes_read > 0) 
        {
            client->buffer[bytes_read] = '\0';

            // Verrouiller le mutex pour modifier l'historique des messages
            pthread_mutex_lock(&message_mutex);
            strcpy(server_message, client->buffer);

            // Ajouter le message reçu à l'historique
            addMessageToHistory(client->buffer, SERVER_MSG);

            new_server_message = true;
            pthread_mutex_unlock(&message_mutex);

            printf("Client received: %s\n", server_message);
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
    return NULL;
}