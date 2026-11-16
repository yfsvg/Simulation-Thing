#include <algorithm>

#include "raylib.h"

#include "environment.hpp"
#include "camera.hpp"

Camera2D camera = {0};
float cameraSpeed = 400.0f;


void initCamera() {
    camera.target = (Vector2){0.0f, 0.0f};
    camera.offset = (Vector2) {400.0f, 400.0f};
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

    // Dragging the map with the mouse. Divided by zoom so the ground stays stuck under the cursor
    if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
        Vector2 mouseDelta = GetMouseDelta();
        camera.target.x -= mouseDelta.x / camera.zoom;
        camera.target.y -= mouseDelta.y / camera.zoom;
    }

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