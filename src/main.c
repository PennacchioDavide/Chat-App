#include <raylib.h>
#define RAYGUI_IMPLEMENTATION
#include "../include/raygui.h"
#include "../include/server_client.h"
#include "../include/thread.h"

#define MAX_MESSAGES 100

typedef enum { CLIENT_MSG, SERVER_MSG } MessageType;

typedef struct {
    char message[BUFFER_SIZE];
    MessageType type;
} Message;

Message message_history[MAX_MESSAGES];
int message_count = 0;

void addMessageToHistory(const char *message, MessageType type) {
    if (message_count < MAX_MESSAGES) {
        strncpy(message_history[message_count].message, message, BUFFER_SIZE);
        message_history[message_count].type = type;
        message_count++;
    } else {
        // Shift messages to make space for new ones
        for (int i = 1; i < MAX_MESSAGES; i++) {
            strncpy(message_history[i - 1].message, message_history[i].message, BUFFER_SIZE);
            message_history[i - 1].type = message_history[i].type;
        }
        strncpy(message_history[MAX_MESSAGES - 1].message, message, BUFFER_SIZE);
        message_history[MAX_MESSAGES - 1].type = type;
    }
}


int main(int argc, char *argv[]) {
    int is_server = 0; // 0 = client, 1 = server

    // Check if server mode is passed as argument
    if (argc > 1 && strcmp(argv[1], "server") == 0) {
        is_server = 1;
    }

    if (is_server) {
        struct socket_t *server = createServer();
        runServer(server);
        free(server);
        return 0;
    }

    InitWindow(1280, 720, "Chat App");
    SetTargetFPS(60);

    char input_buffer[256] = "";
    pthread_t client_tid;

    // Launch client thread
    pthread_create(&client_tid, NULL, client_thread, NULL);

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(DARKGRAY);

        DrawText("Chats:", 20, 20, 30, RAYWHITE);
        DrawLine(200, 720, 200, 0, RAYWHITE); // Vertical line
        DrawLine(0, 70, 200, 70, RAYWHITE); // Horizontal line

        if (GuiTextBox((Rectangle){220, 660, 900, 30}, input_buffer, 256, true) || 
            (IsKeyPressed(KEY_ENTER) && strcmp(input_buffer, "") != 0)) {

            pthread_mutex_lock(&message_mutex);
            strcpy(client_message, input_buffer);
            new_client_message = true;
            addMessageToHistory(input_buffer, CLIENT_MSG); 
            strcpy(input_buffer, ""); 
            pthread_mutex_unlock(&message_mutex);
        }

        if (GuiButton((Rectangle){1120, 660, 140, 30}, "Send") || IsKeyPressed(KEY_ENTER)) {
            pthread_mutex_lock(&message_mutex);
            strcpy(client_message, input_buffer);
            new_client_message = true;
            addMessageToHistory(input_buffer, CLIENT_MSG);
            strcpy(input_buffer, "");
            pthread_mutex_unlock(&message_mutex);
        }

        // If there's a new server message, add it to history
        if (new_server_message) {
            addMessageToHistory(server_message, SERVER_MSG);
            new_server_message = false;
        }

        // Render message history
        for (int i = 0; i < message_count; i++) {
            int y_position = 80 + i * 30;
            if (message_history[i].type == CLIENT_MSG) {
                DrawText(message_history[i].message, 220, y_position, 20, LIGHTGRAY);
            } else if (message_history[i].type == SERVER_MSG) {
                DrawText(message_history[i].message, 220, y_position, 20, GREEN);
            }
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
