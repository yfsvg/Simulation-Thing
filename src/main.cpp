#include <math.h>
#include <string>
#include <deque>

#include "raylib.h"
#include "raymath.h"

int main(void) {

    InitWindow(800, 800, "Simulation");
    SetTargetFPS(60);

    while (!WindowShouldClose()) {

        BeginDrawing();
        ClearBackground(Color{25, 25, 25, 1});
        EndDrawing();
    }

    CloseWindow();

    return 0;
}