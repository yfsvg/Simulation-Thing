#include <math.h>
#include <string>
#include <algorithm>
#include <vector>
#include <random>
#include <cmath>

#include "raylib.h"
#include "raymath.h"

#include "main.hpp"
#include "unit.hpp"
#include "incentives.hpp"

std::vector<std::vector<bool>> cuddleGrid;
float mapWidth = 10000;
float mapHeight = 10000;

std::vector<std::vector<bool>> exploredTiles;

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

    // highligh already explored parts. Include fix
    for (int row = 0; row < mapHeight / 25; row++) {
        for (int col = 0; col < mapWidth / 25; col++) {

            if (!exploredTiles[row][col]) {
                DrawRectangle(col*25 - mapWidth/2, row*25 - mapHeight/2, 25, 25, {0, 0, 0, 100});
            } else {

            }

        };
    };

    // Draw center
    DrawRectangle(-10, -10, 20, 20, WHITE);
    
}


int randomNumMain(int min, int max) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<int> distrib(min, max);
    return distrib(gen);
}

int main(void) {
    SetConfigFlags(FLAG_WINDOW_RESIZABLE); 
    InitWindow(800, 800, "Simulation");
    SetTargetFPS(60);

    std::vector<unit> allUnits;
    std::vector<incentives> allIncentives;
    // Just the IDs, use the IDs as reference to get the actual data from the full incentive array
    int totalIncentives = 100;

    for (int i = 0; i < totalIncentives; i++) {
        Vector2 newPos = { (float)randomNumMain(mapWidth * -0.5f, mapWidth * 0.5f), (float)randomNumMain(mapHeight * -0.5f, mapHeight * 0.5f) };
        incentives newIncentive = incentives((int)(allIncentives.size() + 1), newPos, randomNumMain(0, 1000)/100.0f, randomNumMain(0, 1000)/100.0f);
        allIncentives.push_back(newIncentive);
    }

    exploredTiles.clear();
    for (int row = 0; row < mapHeight / 25; row++) {
        std::vector<bool> toPush;
        for (int col = 0; col < mapWidth / 25; col++) {
            toPush.push_back(false);
        };
        exploredTiles.push_back(toPush);
    };


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
                    oneUnit.goExplore((int)allUnits.size());
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

                Vector2 unitPos = unitToDraw.getPosition();
                for (incentives& indivIncentive : allIncentives) {
                    // Is it within 120 pixels? If so, remove incentive
                    Vector2 incentivePos = indivIncentive.getPosition();

                    if (pow(unitPos.x - incentivePos.x, 2.0f) + pow(unitPos.y - incentivePos.y, 2) < pow(120, 2) && !indivIncentive.isFound()) {
                        indivIncentive.die();
                        // Tell the unit qbout it you found one
                    }
                }
            }

            for (incentives& indivIncentive : allIncentives) {
                indivIncentive.draw();
            }
        EndMode2D();

        
        // Detect to see if any units are close enough to any of the incentives.
        

        EndDrawing();
    }

    CloseWindow();

    return 0;
}