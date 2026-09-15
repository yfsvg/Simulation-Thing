#include <math.h>
#include <string>
#include <random>
#include <cmath>
#include <vector>
#include <algorithm>
#include <set>

#include "raylib.h"
#include "raymath.h"

#include "main.hpp"
#include "unit.hpp"

std::vector<incentives> sharedIncentivesFound;

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

    currentPositionalGoal = { (float)randomNum(-50, 50), (float)randomNum(-50, 50) };
    currentDirectionalGoal = std::atan2(currentPositionalGoal.y - position.y, currentPositionalGoal.x - position.x) * (180.0f / 3.1415926535897932384f);

    idAsString = std::to_string(id);
    idAsString = idAsString.substr(0, idAsString.find('.'));

    currentState = unitState::HangingOut;

    personalSpace = 125.0f; // radius at which repulsion kicks in
    repelStrength = 3.0f; // How powerful the repulsion is. Probably should be reducing during cuddling or returning stages so they are more willing to closer
    vision = 5;
    explorationRadius = 3000;

    incentivesFoundInSession.clear();
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


    float goalX = (distance > 0.0001f) ? dx / distance : 0.0f;
    float goalY = (distance > 0.0001f) ? dy / distance : 0.0f;

    // MRE-CCIC's rendezvous/comms structure, feeding into an MRTA task allocation step at each rendezvous


    Vector2 repulsionForces = {0.0f, 0.0f};

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

    if (distance < 10.0f && currentState == unitState::CuddlingUp) {
        direction = 90;
        position.x = currentPositionalGoal.x;
        position.y = currentPositionalGoal.y;
        velocity = 0;
    } else {
        currentDirectionalGoal = std::atan2(desiredY, desiredX) * (180.0f / 3.1415926535897932384f);
    }
    // the best one
    float diff = currentDirectionalGoal - direction;
    while (diff < -180.0f) diff += 360.0f;
    while (diff > 180.0f) diff -= 360.0f;

    float desiredSpeed = (distance > 20.0f) ? fmaxf(200.0f * cosf(diff * (3.1415926535897932384f / 180.0f)), 0.0f) : 0.0f;
    acceleration = (desiredSpeed - velocity) * 2.0f;

    direction += diff * 2.0f * deltaTime;


    if (currentState == unitState::Exploring) {
        personalSpace = 125.0f;
        repelStrength = 3.0f;
    } else if (currentState == unitState::CuddlingUp) {
        personalSpace = 0.0f;
        repelStrength = 0.0f;
    } else if (currentState == unitState::Returning) {
        personalSpace = 22.5f;
        repelStrength = 1.0f;
    }
    

    if (currentState == unitState::HangingOut && distance < 20.0f) {
        currentPositionalGoal = { (float)randomNum(-50, 50), (float)randomNum(-50, 50) };
    } else if (distance < 40.0f) {
        if (currentState == unitState::Exploring) {
            goExploreTarget((int)(allOtherUnits.size()));
        }
    } else if (distance < 50.0f && currentState == unitState::Returning) {
        currentState = unitState::CuddlingUp;
        // now that they are sufficiently close, make them go towards the innermost vacant "false" spot in the cuddlegrid. Each grid is separated 50x50.
    }


    velocity += acceleration * deltaTime;
    position.x += velocity * cosf(direction * (3.1415926535897932384f / 180.0f)) * deltaTime;
    position.y += velocity * sinf(direction * (3.1415926535897932384f / 180.0f)) * deltaTime;


    // mark newly wandered areas as explored with a radius of vision tiles
    int centerVertical = (int)((position.x + mapWidth * 0.5f) / 25);
    int centerHorizontal = (int)((position.y + mapHeight * 0.5f) / 25);

    for (int row = std::max(0, centerHorizontal - vision); row <= std::min((int)(mapHeight / 25) - 1, centerHorizontal + vision); ++row) {
        for (int col = std::max(0, centerVertical - vision); col <= std::min((int)(mapWidth / 25) - 1, centerVertical + vision); ++col) {

            int dx = col - centerVertical;
            int dy = row - centerHorizontal;

            if (dx * dx + dy * dy <= vision * vision) {
                exploredTiles[row][col] = true;
            }
        }
    }

}


void unit::claimCuddleSpot() {
    int gridSize = 50;
    int n = (int)cuddleGrid.size();
    if (n == 0) return;

    // center of grid: even-sized grid centers on 0,0; odd-sized centers on 25,25
    int centerIdx = n / 2;

    float bestDistSq = -1.0f;
    int bestRow = -1, bestCol = -1;

    for (int row = 0; row < n; row++) {
        for (int col = 0; col < (int)cuddleGrid[row].size(); col++) {
            if (cuddleGrid[row][col]) continue; // occupied

            float cellX = (col - (n / 2)) * (float)gridSize;
            float cellY = (row - (n / 2)) * (float)gridSize;
    
            float dSq = cellX * cellX + cellY * cellY;

            if (bestDistSq < 0.0f || dSq < bestDistSq) {
                bestDistSq = dSq;
                bestRow = row;
                bestCol = col;
            }
        }
    }

    // if (bestRow == -1) {return;

    // Claiming
    cuddleGrid[bestRow][bestCol] = true;

    currentPositionalGoal.x = (bestCol - (float)(n / 2)) * (float)gridSize;
    currentPositionalGoal.y = (bestRow - (float)(n / 2)) * (float)gridSize;
}

void unit::cuddle() {
    if (currentState == unitState::Exploring) {
        currentState = unitState::Returning;
        for (int i = 0; i < incentivesFoundInSession.size(); i++) {
            // Not necessary to check for uniqueness because each incentive can only be discovered once anyways
            sharedIncentivesFound.push_back(incentivesFoundInSession[i]);
        }
        claimCuddleSpot();
    }
}



void unit::goExploreTarget(int totalUnits) {
    if (totalUnits <= 0) totalUnits = 1;

    float sliceDeg = 360.0f / (float)totalUnits;
    float sliceStart = (id - 1.0f) * sliceDeg;

    float angleDeg = sliceStart + randomNum(0, (int)sliceDeg) + 0.0f;
    float angleRad = angleDeg * (3.1415926535897932384f / 180.0f);

    float r = explorationRadius * std::sqrt(randomNum(0, 1000) / 1000.0f);

    currentPositionalGoal = Vector2{
        r * cosf(angleRad),
        r * sinf(angleRad)
    };
}

void unit::goExplore(int totalUnits) {
    currentState = unitState::Exploring;
    goExploreTarget(totalUnits);
}

void unit::findOne(incentives incentiveFound) {
    incentivesFoundInSession.push_back(incentiveFound);
}