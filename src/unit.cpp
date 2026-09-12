#include <math.h>
#include <string>

#include "raylib.h"
#include "raymath.h"
#include "unit.hpp"

unit::unit(int inputId, Vector2 inputPosition) {
    position = inputPosition;
    id = inputId;
    size = 45;
    // in degrees. Start from right, go ccw
    direction = 0;
    velocity = 0;
    acceleration = 0;
}

void unit::draw() {
    DrawRectangle(position.x * -0.5 * size, position.y * -0.5 * size, size, size, WHITE);
    
}