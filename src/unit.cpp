#include <math.h>
#include <string>
#include <random>
#include <cmath>
#include <vector>

#include "raylib.h"
#include "raymath.h"
#include "unit.hpp"

int randomNum(int min, int max) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<int> distrib(min, max);
    return distrib(gen);
}


unit::unit(float inputId, Vector2 inputPosition) {
    position = inputPosition;
    id = inputId;
    size = 45.0f;
    // in degrees. Start from right, go ccw
    direction = randomNum(0, 359) + 0.0f;
    velocity = randomNum(0, 20) + 0.0f;
    acceleration = 0;

    currentPositionalGoal = {(float)randomNum(-2500, 2500), (float)randomNum(-2500, 2500)};
    currentDirectionalGoal = std::atan2(currentPositionalGoal.y - position.y, currentPositionalGoal.x - position.x) * (180.0f / 3.1415926535897932384f);

    idAsString = std::to_string(id);
    idAsString = idAsString.substr(0, idAsString.find('.'));

    currentState = unitState::Exploring;

}

Vector2 unit::getPosition() {
    return position;
}

float unit::getID() {
    return id;
}




void unit::draw() {
    Rectangle drawingUnit = {position.x, position.y, size, size};
    Vector2 drawingUnitOrigin = {size / 2.0f, size / 2.0f};
    DrawRectanglePro(drawingUnit, drawingUnitOrigin, direction, WHITE);

    int fontSize = 20;
    int textWidth = MeasureText(idAsString.c_str(), fontSize);
    DrawText(idAsString.c_str(), position.x - (textWidth / 2.0f), position.y - (20 / 2.0f), fontSize, BLACK);

    DrawText(idAsString.c_str(), currentPositionalGoal.x - 5, currentPositionalGoal.y - 5, 20, RED);
}




void unit::tickUpdate(float deltaTime, const std::vector<unit>& allOtherUnits) {
    // !!!!EXPLORING BEHAVIOR!!!!
    // !!!!EXPLORING BEHAVIOR!!!!

    Vector2 myPos = getPosition();

    float dx = currentPositionalGoal.x - myPos.x;
    float dy = currentPositionalGoal.y - myPos.y;
    float distance = std::sqrt(dx * dx + dy * dy);

    // Fix: Normalize the goal direction so it's comparable in scale to repulsion
    float goalX = (distance > 0.0001f) ? dx / distance : 0.0f;
    float goalY = (distance > 0.0001f) ? dy / distance : 0.0f;





    Vector2 repulsionForces = {0.0f, 0.0f};
    const float personalSpace = 150.0f; // radius at which repulsion kicks in
    const float repelStrength = 4.0f; // How powerful the repulsion is. Probably should be reducing during cuddling or returning stages so they are more willing to closer

    for (unit other : allOtherUnits) {
        if (other.getID() == id) {
            continue;
        }

        Vector2 otherPositions = other.getPosition();
        float distanceToOthersSq = pow(myPos.x - otherPositions.x, 2.0f) + pow(myPos.y - otherPositions.y, 2.0f);

        if (distanceToOthersSq < 0.0001f) {
            distanceToOthersSq = 0.0001f;
        }
        float oDist = std::sqrt(distanceToOthersSq);

        if (oDist < personalSpace) {
            // Dynamic pushing based on how close the distance is to the set personal space, that way they dont overreact and explode just from getting somewhat close to each other
            float falloff = (personalSpace - oDist) / personalSpace;
            repulsionForces.x += ((myPos.x - otherPositions.x) / oDist) * falloff * repelStrength;
            repulsionForces.y += ((myPos.y - otherPositions.y) / oDist) * falloff * repelStrength;
        }

    }

    // Update the direction goal dynamically based on avoidance previously
    float desiredX = goalX + repulsionForces.x;
    float desiredY = goalY + repulsionForces.y;

    currentDirectionalGoal = std::atan2(desiredY, desiredX) * (180.0f / 3.1415926535897932384f);

    float diff = currentDirectionalGoal - direction;
    while (diff < -180.0f) diff += 360.0f;
    while (diff > 180.0f) diff -= 360.0f;

    float desiredSpeed = (distance > 20.0f) ? fmaxf(200.0f * cosf(diff * (3.1415926535897932384f / 180.0f)), 0.0f) : 0.0f;
    acceleration = (desiredSpeed - velocity) * 2.0f;

    direction += diff * 2.0f * deltaTime;

    if (distance < 40.0f && currentState == unitState::Exploring) {
        currentPositionalGoal = { (float)randomNum(-2500, 2500), (float)randomNum(-2500, 2500) };
    }
    


    velocity += acceleration * deltaTime;
    position.x += velocity * cosf(direction * (3.1415926535897932384f / 180.0f)) * deltaTime;
    position.y += velocity * sinf(direction * (3.1415926535897932384f / 180.0f)) * deltaTime;
}

void unit::cuddle() {
    float closestSquare = pow(std::round(std::sqrt(id)), 2);
}