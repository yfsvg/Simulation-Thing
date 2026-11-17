#include <algorithm>

#include "raylib.h"

#include "environment.hpp"
#include "camera.hpp"

// Settings, not game state, so these stay here instead of in Game
static const float cameraSpeed = 400.0f;


static const int panButton = MOUSE_BUTTON_LEFT;


void initCamera(Camera2D& camera) {
    camera.target = (Vector2){0.0f, 0.0f};
    camera.offset = (Vector2){400.0f, 400.0f};
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;
}



void cameraMovement(Camera2D& camera, const GameMap& map) {
    float dt = GetFrameTime();

    // Regular offset
    if (IsKeyDown(KEY_W)) camera.target.y -= cameraSpeed * dt;
    if (IsKeyDown(KEY_S)) camera.target.y += cameraSpeed * dt;
    if (IsKeyDown(KEY_A)) camera.target.x -= cameraSpeed * dt;
    if (IsKeyDown(KEY_D)) camera.target.x += cameraSpeed * dt;

    // Dragging the map with the mouse. Divided by zoom so the ground stays stuck under the cursor
    if (IsMouseButtonDown(panButton)) {
        Vector2 mouseDelta = GetMouseDelta();
        camera.target.x -= mouseDelta.x / camera.zoom;
        camera.target.y -= mouseDelta.y / camera.zoom;
    }

    camera.target.x = std::clamp(camera.target.x, -0.5f * map.width, 0.5f * map.width);
    camera.target.y = std::clamp(camera.target.y, -0.5f * map.height, 0.5f * map.height);

    // zooming in and out
    if (IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT) || GetMouseWheelMove() > 0) camera.zoom += 1.0f * dt;
    if (IsKeyDown(KEY_SPACE) || GetMouseWheelMove < 0) camera.zoom -= 1.0f * dt;

    // Clamp zoom levels to prevent inverse/extreme values
    camera.zoom = std::clamp(camera.zoom, 0.1f, 5.0f);
}