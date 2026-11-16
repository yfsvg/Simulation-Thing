#include <algorithm>
#include <cmath>
#include <string>
#include <vector>

#include "raylib.h"

#include "utils.hpp"
#include "environment.hpp"
#include "unit.hpp"

std::vector<std::string> allUnitTypes = {
    "Scout",
    "Driller",
    "Fabricator",
    "Hauler"
};

static const Color unitTypeColors[] = {WHITE, RED, GREEN, BLUE};


unit::unit(int inputId, Vector2 inputPosition, int typeInput) {
    position = inputPosition;
    id = inputId;
    idAsString = std::to_string(id);
    currentType = typeInput;

    size = 45.0f;
    direction = (float)randomNum(0, 359);
    velocity = (float)randomNum(0, 20);
    acceleration = 0.0f;
    vision = 5;

    personalSpace = 150.0f;
    repelStrength = 4.0f;

    idleAnchor = inputPosition;
    idleRadius = 100.0f;
    pickIdleGoal();
    currentDirectionalGoal = direction;
}


Vector2 unit::getPosition() const {
    return position;
}

int unit::getID() const {
    return id;
}


void unit::draw() const {
    Rectangle drawingUnit = {position.x, position.y, size, size};
    Vector2 drawingUnitOrigin = {size / 2.0f, size / 2.0f};
    DrawRectanglePro(drawingUnit, drawingUnitOrigin, direction, unitTypeColors[currentType]);

    int fontSize = 20;
    int textWidth = MeasureText(idAsString.c_str(), fontSize);
    DrawText(idAsString.c_str(), position.x - (textWidth / 2.0f), position.y - (fontSize / 2.0f), fontSize, BLACK);
}


// Idle behavior: wander to random points around the idle anchor
void unit::pickIdleGoal() {
    int r = (int)idleRadius;
    currentPositionalGoal = {
        idleAnchor.x + (float)randomNum(-r, r),
        idleAnchor.y + (float)randomNum(-r, r)
    };
}


void unit::tickUpdate(float deltaTime, const std::vector<unit>& allUnits) {
    float dx = currentPositionalGoal.x - position.x;
    float dy = currentPositionalGoal.y - position.y;
    float distance = std::sqrt(dx * dx + dy * dy);

    float goalX = (distance > 0.0001f) ? dx / distance : 0.0f;
    float goalY = (distance > 0.0001f) ? dy / distance : 0.0f;

    // Keep out of each other's personal space
    Vector2 repulsionForces = {0.0f, 0.0f};

    for (const unit& other : allUnits) {
        if (other.id == id) {
            continue;
        }

        float offX = position.x - other.position.x;
        float offY = position.y - other.position.y;
        float oDist = std::sqrt(std::max(offX * offX + offY * offY, 0.0001f));

        if (oDist < personalSpace) {
            // Dynamic pushing based on how close the distance is to the set personal space, that way they dont overreact and explode just from getting somewhat close to each other
            float falloff = (personalSpace - oDist) / personalSpace;
            repulsionForces.x += (offX / oDist) * falloff * repelStrength;
            repulsionForces.y += (offY / oDist) * falloff * repelStrength;
        }
    }

    // Steer towards the goal, adjusted by the avoidance above
    currentDirectionalGoal = std::atan2(goalY + repulsionForces.y, goalX + repulsionForces.x) * RAD2DEG;

    float diff = currentDirectionalGoal - direction;
    while (diff < -180.0f) diff += 360.0f;
    while (diff > 180.0f) diff -= 360.0f;

    float desiredSpeed = (distance > 20.0f) ? fmaxf(200.0f * cosf(diff * DEG2RAD), 0.0f) : 0.0f;
    acceleration = (desiredSpeed - velocity) * 2.0f;

    direction += diff * 2.0f * deltaTime;

    // Arrived, go wander somewhere else nearby
    if (distance < 20.0f) {
        pickIdleGoal();
    }

    velocity += acceleration * deltaTime;
    position.x += velocity * cosf(direction * DEG2RAD) * deltaTime;
    position.y += velocity * sinf(direction * DEG2RAD) * deltaTime;

    // mark newly wandered areas as explored with a radius of vision tiles
    markExplored(position, vision);
}
