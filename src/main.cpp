#include <vector>

#include "raylib.h"
#include "raymath.h"

#include "environment.hpp"
#include "camera.hpp"
#include "incentives.hpp"
#include "unit.hpp"
#include "groupState.hpp"
#include "debug.hpp"


// Pressing C makes units, adds them to the pile.
// You're welcome for this!


static void spawnUnit(std::vector<unit>& allUnits) {
    int inputType = 0;

    if (IsKeyDown(KEY_TWO)) {inputType = 1;}
    else if (IsKeyDown(KEY_THREE)) {inputType = 2;}
    else if (IsKeyDown(KEY_FOUR)) {inputType = 3;}
    else if (IsKeyDown(KEY_FIVE)) {inputType = 4;}

    unit newUnit = unit((float)(allUnits.size() + 1), (Vector2){0, 0}, inputType);
    newUnit.setGroupState(groupBehavior, (int)allUnits.size() + 1);
    allUnits.push_back(newUnit);
}

static void handleInput(std::vector<unit>& allUnits) {
    if (IsKeyPressed(KEY_C)) {
        spawnUnit(allUnits);
    }


    if (IsKeyPressed(KEY_X)) {
        advanceGroupState(allUnits);
    }
}





// A huge portion of code 
int main(void) {
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(1200, 800, "Simulation");
    SetTargetFPS(60);

    std::vector<unit> allUnits;
    std::vector<incentives> allIncentives;
    // Just the IDs, use the IDs as reference to get the actual data from the full incentive array
    int totalIncentives = 100;

    spawnIncentives(allIncentives, totalIncentives);
    initExploredTiles();
    initCamera();



    while (!WindowShouldClose()) {
        cameraMovement();

        BeginDrawing();
        ClearBackground(Color{25, 25, 25, 255});

        handleInput(allUnits);

        BeginMode2D(camera);
            drawBG();
            Vector2 mouseWorldPosition = GetScreenToWorld2D(GetMousePosition(), camera);
            float deltaTime = GetFrameTime();
            for (unit& unitToDraw : allUnits) {
                unitToDraw.tickUpdate(deltaTime, allUnits);
                unitToDraw.draw();
                unitToDraw.lookForIncentives(allIncentives);
            }

            for (incentives& indivIncentive : allIncentives) {
                indivIncentive.draw(mouseWorldPosition);
            }
        EndMode2D();

        drawDebugInfo(allUnits, allIncentives);

        EndDrawing();
    }

    CloseWindow();

    return 0;
}
