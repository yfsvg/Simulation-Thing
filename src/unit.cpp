#include <math.h>
#include <string>
#include <cmath>
#include <vector>
#include <algorithm>
#include <set>

#include "raylib.h"
#include "raymath.h"

#include "main.hpp"
#include "unit.hpp"
#include "unitMind.hpp"

// A NEW SYSTEM FOR POSITIONAL TARGETS!!
// rather than just a point to go to, they have a list of points to go to as priorities so they can be programmed to do
// multiple tasks during each meetup. They also have a list of time that they need to be there for to do their tasks.


// how about the greedy system and then a comparison to a system that divides the interest by the how scarce the bots 
// are in that category. For example, if a swarm has a lot of type 1 and only 3 type 2s, theyd like to take on a task 
// with interest 4 that needs 2 type 1s more than a task with interest 5 that needs all 3 type 2s
// Could be a potentially interesting idea, maybe implement later as a comparison.


unit::unit(float inputId, Vector2 inputPosition, int typeInput) {
    position = inputPosition;
    id = inputId;
    size = 45.0f;
    currentType = typeInput;
    // in degrees. Start from right, go ccw
    direction = randomNum(0, 359) + 0.0f;
    velocity = randomNum(0, 20) + 0.0f;
    acceleration = 0;

    currentPositionalGoal = { (float)randomNum(-50, 50), (float)randomNum(-50, 50) };
    currentDirectionalGoal = std::atan2(currentPositionalGoal.y - position.y, currentPositionalGoal.x - position.x) * (180.0f / 3.1415926535897932384f);

    futurePositionalGoals.clear();
    timeInSSpentAtFutureGoal.clear();

    idAsString = std::to_string(id);
    idAsString = idAsString.substr(0, idAsString.find('.'));

    personalSpace = 125.0f; // radius at which repulsion kicks in
    repelStrength = 3.0f; // How powerful the repulsion is. Probably should be reducing during cuddling or returning stages so they are more willing to closer
    vision = 5;
    explorationRadius = 3000;

    investigatingOrNo = false;

    incentivesFoundInSession.clear();

    // Units always start out while hanging out near the center until the first X press moves the whole group into the next stage
    currentState = groupState::HangingOut;
    hasArrivedAtCuddle = false;
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
    std::vector<Color> colorsOfCourse = {WHITE, RED, GREEN, BLUE, YELLOW};
    if (hasArrivedAtCuddle) {
        DrawRectanglePro(drawingUnit, drawingUnitOrigin, 90, colorsOfCourse[currentType]);
    } else {
        DrawRectanglePro(drawingUnit, drawingUnitOrigin, direction, colorsOfCourse[currentType]);
    }


    int fontSize = 20;
    int textWidth = MeasureText(idAsString.c_str(), fontSize);
    DrawText(idAsString.c_str(), position.x - (textWidth / 2.0f), position.y - (20 / 2.0f), fontSize, BLACK);

    DrawText(idAsString.c_str(), currentPositionalGoal.x - 5, currentPositionalGoal.y - 5, 20, RED);
}








void unit::tickUpdate(float deltaTime, const std::vector<unit>& allOtherUnits) {

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

    // Snapping behavior, one of the biggest issues was 
    if (distance < 17.0f && (currentState == groupState::CuddlingProcess ||
        currentState == groupState::CuddlingBeforeExploration ||
        currentState == groupState::CuddlingBeforeInvestigation ||
        currentState == groupState::Returning)) {
        direction = 90;
        position.x = currentPositionalGoal.x;
        position.y = currentPositionalGoal.y;
        velocity = 0;
        if (!hasArrivedAtCuddle) {
            hasArrivedAtCuddle = true;
        }
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


    if (currentState == groupState::Exploring || currentState == groupState::Investigating) {
        personalSpace = 150.0f;
        repelStrength = 4.0f;
        hasArrivedAtCuddle = false;
    } else if (currentState == groupState::CuddlingProcess || currentState == groupState::CuddlingBeforeExploration || currentState == groupState::CuddlingBeforeInvestigation) {
        personalSpace = 0.0f;
        repelStrength = 0.0f;
    } else if (currentState == groupState::Returning) {
        personalSpace = 22.5f;
        repelStrength = 2.0f;
    }

    if (currentState == groupState::HangingOut && distance < 20.0f) {
        currentPositionalGoal = { (float)randomNum(-50, 50), (float)randomNum(-50, 50) };
    } else if (distance < 40.0f) {
        if (currentState == groupState::Exploring) {
            goExploreTarget((int)(allOtherUnits.size()));
        } 
        
        if (currentState == groupState::Investigating) {
            // In the future, IN HERE we make the guy wait
            if (futurePositionalGoals.empty()) {
                currentState = groupState::HangingOut;
                currentPositionalGoal = {0.0f, 0.0f};
            } else {
                futurePositionalGoals.erase(futurePositionalGoals.begin());
                if (!timeInSSpentAtFutureGoal.empty()) {
                    timeInSSpentAtFutureGoal.erase(timeInSSpentAtFutureGoal.begin());
                }

                // Assume that the one in tick checker will handle it maybe?
                if (!futurePositionalGoals.empty()) {
                    currentPositionalGoal = futurePositionalGoals[0];
                }
            }
            // knock off the incentive they just went to, and then set positional goal back to the next item in line
            // If incentive list is size 0, then start hanging out back at the center to charge up and chill
        }
    } else if (distance < 50.0f && currentState == groupState::Returning) {
        currentState = groupState::CuddlingProcess;
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

    if (bestRow == -1) return;

    // Claiming
    cuddleGrid[bestRow][bestCol] = true;

    currentPositionalGoal.x = (bestCol - (float)(n / 2)) * (float)gridSize;
    currentPositionalGoal.y = (bestRow - (float)(n / 2)) * (float)gridSize;
}

void unit::cuddle() {
    if (currentState == groupState::Exploring || currentState == groupState::Investigating) {
        currentState = groupState::Returning;
        for (int i = 0; i < incentivesFoundInSession.size(); i++) {
            bool alreadyShared = std::find_if(
                sharedIncentivesFound.begin(),
                sharedIncentivesFound.end(),
                [&](const incentives& foundIncentive) {
                    return foundIncentive.id == incentivesFoundInSession[i].id;
                }
            ) != sharedIncentivesFound.end();
            if (!alreadyShared) sharedIncentivesFound.push_back(incentivesFoundInSession[i]);
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

void unit::goExplore(int totalUnits, bool investigatingOrNoInput) {
    currentState = groupState::Exploring;
    investigatingOrNo = investigatingOrNoInput;
    if (!investigatingOrNo) {
        goExploreTarget(totalUnits);
    } else {
        // Go to 
    }
}

void unit::findOne(incentives incentiveFound) {
    incentivesFoundInSession.push_back(incentiveFound);
}

void unit::addToGoal(Vector2 goalAdd) {
    if (futurePositionalGoals.empty()) {
        currentPositionalGoal = goalAdd;
    }

    futurePositionalGoals.push_back(goalAdd);
}



bool unit::hasReturnedToCuddle() const {
    return hasArrivedAtCuddle &&
        (currentState == groupState::CuddlingBeforeExploration ||
         currentState == groupState::CuddlingBeforeInvestigation ||
         currentState == groupState::CuddlingProcess);
}



// This is now the ONLY place that drives a unit's phase from the outside.
// changed from having stuff inside of unitstate / unit.cpp itself change because that can mess stuf fup

void unit::setGroupState(groupState inputGroupBehavior, int totalUnits) {
    switch (inputGroupBehavior) {
        case groupState::CuddlingBeforeExploration:
        case groupState::CuddlingBeforeInvestigation:
            if (currentState == groupState::Exploring || currentState == groupState::Investigating) {
                cuddle();
            } else {
                currentState = groupState::Returning;
                claimCuddleSpot();
            }
            currentState = inputGroupBehavior;
            break;

        case groupState::Exploring:
            goExplore(totalUnits, false);
            break;

        case groupState::Investigating:
            // add with the incentive list later
            goExplore(totalUnits, true);
            break;

        case groupState::HangingOut:
        case groupState::Returning:
        case groupState::CuddlingProcess:
        default:
            currentState = inputGroupBehavior;
            break;
    }
}
