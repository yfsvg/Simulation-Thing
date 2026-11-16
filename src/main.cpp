#include <vector>

#include "raylib.h"

#include "environment.hpp"
#include "camera.hpp"
#include "unit.hpp"


// Pressing C makes units, adds them to the pile.
// Hold 2 / 3 / 4 while pressing C to pick the type (see allUnitTypes), otherwise its a scout.

static void spawnUnit(std::vector<unit>& allUnits) {
    int inputType = 0;

    if (IsKeyDown(KEY_TWO)) {inputType = 1;}
    else if (IsKeyDown(KEY_THREE)) {inputType = 2;}
    else if (IsKeyDown(KEY_FOUR)) {inputType = 3;}

    allUnits.push_back(unit((int)allUnits.size() + 1, (Vector2){0, 0}, inputType));
}

static void handleInput(std::vector<unit>& allUnits) {
    if (IsKeyPressed(KEY_C)) {
        spawnUnit(allUnits);
    }
}


int main(void) {
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(1200, 800, "Simulation");
    SetTargetFPS(60);

    std::vector<unit> allUnits;

    initExploredTiles();
    initCamera();

    while (!WindowShouldClose()) {
        float deltaTime = GetFrameTime();

        cameraMovement();
        handleInput(allUnits);

        for (unit& oneUnit : allUnits) {
            oneUnit.tickUpdate(deltaTime, allUnits);
        }

        BeginDrawing();
        ClearBackground(Color{25, 25, 25, 255});

        BeginMode2D(camera);
            drawBG();
            for (const unit& unitToDraw : allUnits) {
                unitToDraw.draw();
            }
        EndMode2D();

        EndDrawing();
    }

    CloseWindow();

    return 0;
}
