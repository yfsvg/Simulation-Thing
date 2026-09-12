#include <math.h>
#include <string>
#include <algorithm>
#include <vector>

#include "raylib.h"
#include "raymath.h"

#include "unit.hpp"

extern float mapWidth = 1000;
extern float mapHeight = 1000;

Camera2D camera = { 0 };
float cameraSpeed = 400.0f; // Speed in pixels per second

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
    if (camera.zoom < 0.1f) camera.zoom = 0.1f;
    if (camera.zoom > 5.0f) camera.zoom = 5.0f;
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

        
        BeginMode2D(camera);
            drawBG();
        EndMode2D();

        

        EndDrawing();
    }

    CloseWindow();

    return 0;
}