#include <math.h>
#include <string>
#include <algorithm>
#include <vector>

#include "raylib.h"
#include "raymath.h"

#include "main.hpp"
#include "unit.hpp"

std::vector<std::vector<bool>> cuddleGrid;
float mapWidth = 300;
float mapHeight = 300;

bool exploringOrNo = true;

Camera2D camera = {0};
float cameraSpeed = 400.0f;


void initCamera() {
    camera.target = (Vector2){ 0.0f, 0.0f }; 
    camera.offset = (Vector2){ 400.0f, 400.0f };
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;
}


void cameraMovement() {
    float dt = GetFrameTime();

    // Regular offset
    if (IsKeyDown(KEY_W)) camera.target.y -= cameraSpeed * dt;
    if (IsKeyDown(KEY_S)) camera.target.y += cameraSpeed * dt;
    if (IsKeyDown(KEY_A)) camera.target.x -= cameraSpeed * dt;
    if (IsKeyDown(KEY_D)) camera.target.x += cameraSpeed * dt;

    camera.target.x = std::clamp(camera.target.x, -0.5f*mapWidth, 0.5f*mapWidth);
    camera.target.y = std::clamp(camera.target.y, -0.5f*mapHeight, 0.5f*mapHeight);

    // zooming in and out
    if (IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT)) camera.zoom += 1.0f * dt;
    if (IsKeyDown(KEY_SPACE)) camera.zoom -= 1.0f * dt;

    // Clamp zoom levels to prevent inverse/extreme values
    if (camera.zoom < 0.1f) {
        camera.zoom = 0.1f;
    }
    if (camera.zoom > 5.0f) {
        camera.zoom = 5.0f;
    }
}

void drawBG() {
    int gridSize = 50;
    Color lineColor = {150, 150, 150, 255};

    float minX = mapWidth * -0.5f;
    float maxX = mapWidth * 0.5f;
    float minY = mapHeight * -0.5f;
    float maxY = mapHeight * 0.5f;

    // horizontal lines
    for (int y = (int)minY; y <= (int)maxY; y += gridSize) {
        Vector2 startPos = { minX, (float)y };
        Vector2 endPos = { maxX, (float)y };
        DrawLineV(startPos, endPos, lineColor);
    }

    // vertical lines
    for (int x = (int)minX; x <= (int)maxX; x += gridSize) {
        Vector2 startPos = { (float)x, minY };
        Vector2 endPos = { (float)x, maxY };
        DrawLineV(startPos, endPos, lineColor);
        
    }

    // Draw center
    DrawRectangle(-10, -10, 20, 20, WHITE);
    
}

int main(void) {
    InitWindow(800, 800, "Simulation");
    SetTargetFPS(60);

    std::vector<unit> allUnits;

    initCamera();

    while (!WindowShouldClose()) {
        cameraMovement();

        BeginDrawing();
        ClearBackground(Color{25, 25, 25, 255});

        // Pressing C makes units, adds them to the pile.
        if (IsKeyPressed(KEY_C)) {
            unit newUnit = unit(allUnits.size() + 1, (Vector2){0, 0});
            allUnits.push_back(newUnit);
        }

        // Pressing X makes them go back into cuddle pile
        if (IsKeyPressed(KEY_X)) {
            if (exploringOrNo) {
                float closestSquare = pow(std::ceil(std::sqrt(allUnits.size())), 2);

                cuddleGrid.clear();
                for (int row = 0; row < closestSquare; row++) {
                    std::vector<bool> toPush = {};
                    for (int col = 0; col < closestSquare; col++) {
                        toPush.push_back(false);
                    }
                    cuddleGrid.push_back(toPush);
                }


                for (unit& oneUnit : allUnits) {
                    oneUnit.cuddle();
                }
                

            } else {
                // Start exploring!
                for (unit& oneUnit : allUnits) {
                    oneUnit.goExplore();
                }
            }

            exploringOrNo = !exploringOrNo;

        }
        
        BeginMode2D(camera);
            drawBG();
            float deltaTime = GetFrameTime();
            for (unit& unitToDraw : allUnits) {
                std::vector<unit> allUnitsToSend = allUnits;
                allUnitsToSend.erase(allUnitsToSend.begin() + unitToDraw.getID() - 1);
                unitToDraw.tickUpdate(deltaTime, allUnits);
                unitToDraw.draw();
            }
        EndMode2D();

        
        

        EndDrawing();
    }

    CloseWindow();

    return 0;
}