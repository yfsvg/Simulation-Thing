#include <vector>

#include "raylib.h"

#include "game.hpp"
#include "environment.hpp"
#include "camera.hpp"
#include "unit.hpp"


// Pressing C makes units, adds them to the pile.
// Hold 2 / 3 / 4 while pressing C to pick the type (see allUnitTypes), otherwise its a scout.

static void spawnUnit(Game& game) {
    int inputType = 0;

    if (IsKeyDown(KEY_TWO)) {
        inputType = 1;
    }
    else if (IsKeyDown(KEY_THREE)) {
        inputType = 2;
    }
    else if (IsKeyDown(KEY_FOUR)) {
        inputType = 3;
    }

    game.units.push_back(unit(game.nextUnitId++, (Vector2){0, 0}, inputType));
}


static void handleInput(Game& game) {
    
    if (IsKeyPressed(KEY_C)) {
        spawnUnit(game);
    }
}





int main(void) {
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(1200, 800, "Simulation");
    SetTargetFPS(60);

    Game game;
    initMap(game.map);
    initCamera(game.camera);

    while (!WindowShouldClose()) {
        float deltaTime = GetFrameTime();

        cameraMovement(game.camera, game.map);
        handleInput(game);

        for (unit& oneUnit : game.units) {
            oneUnit.tickUpdate(deltaTime, game.units, game.map);
        }

        BeginDrawing();
        ClearBackground(Color{25, 25, 25, 255});

        BeginMode2D(game.camera);
            drawBG(game.map, game.camera);
            for (const unit& unitToDraw : game.units) {
                unitToDraw.draw();
            }
        EndMode2D();

        EndDrawing();
    }

    CloseWindow();

    return 0;
}