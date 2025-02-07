#include <raylib.h>
#define RAYGUI_IMPLEMENTATION
#include "../include/raygui.h"
#include "../include/server_client.h"
#include <stdbool.h>
#include <string.h>

int main()
{
    InitWindow(1280, 720, "Chat App");
    SetTargetFPS(60);

    char buff[256] = "";
    char message[256] = "";

    bool showText = false;

    int choice;

    while (!WindowShouldClose())
    {
        BeginDrawing();
            
            ClearBackground(DARKGRAY);

            DrawText("Chats:", 20, 20, 30, RAYWHITE);

            DrawLine(200, 720, 200, 0, RAYWHITE); // Vertical Line
            DrawLine(0, 70, 200, 70, RAYWHITE); // Horizontal Line

            GuiTextBox((Rectangle){220, 660, 900, 30}, buff, 100, true); // Input Message Box
            
            if((IsKeyPressed(KEY_ENTER) || GuiButton((Rectangle){1140, 660, 100, 30}, "SEND")) && strcmp(buff, "") != 0) // Transfer text from input buffer to text message and freeing buffer
            {
                showText = true;
                strcpy(message, buff);
                strcpy(buff, "");
            }

            if(showText)
            {
                DrawText(message, 220, 100, 30, RAYWHITE);
            }

        EndDrawing();

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
    }

    CloseWindow();

    return 0;
}