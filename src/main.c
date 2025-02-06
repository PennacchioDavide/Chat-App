#include <raylib.h>
#define RAYGUI_IMPLEMENTATION
#include "../include/raygui.h"
#include "../include/server.h"

int main()
{
    InitWindow(1280, 720, "Chat App");
    SetTargetFPS(60);

    while (!WindowShouldClose())
    {
        BeginDrawing();
            
            ClearBackground(DARKGRAY);

            DrawText("Contacts", 20, 20, 30, RAYWHITE);

            DrawLine(200, 720, 200, 0, RAYWHITE);
            DrawLine(0, 70, 200, 70, RAYWHITE);

        EndDrawing();
    }

    CloseWindow();

    return 0;
}