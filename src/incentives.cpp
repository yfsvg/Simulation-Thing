// The incentives/tasks are placed randomly throughout the massive map. Different variables:
// 1. Difficulty of the task (Time spent at the point doing the thing, at times can take extremely long)
// 2. Interest level (Maybe the task requires additional units to help complete the analysis task faster, ranging from 1-10)
// 3. Type of incentive for narrative
// Current ideas for #3: Resource mining, unusual chemical composition, just anything that could look interesting. Connects to #1 and #2
// 4. Risk level (Doing the tasks themselves usually carries inherent risk im guessing, certain tasks with higher difficulty can cause breakdown more often than others)

// If the bot decides that the task can be completed solo, then it does it (Under a certain difficulty level, for example). calculating based off of how much time is left and how difficult it is


// For the initial exploration burst (ie first day), do a radial sweep in N directions.



// RATHER THAN GOING BACK TO CERTAIN INCENTIVES, THEY SHOULD CREATE AN INTEREST MAP TO PARTS WITH HIGH INTEREST


#include <math.h>
#include <string>
#include <cmath>
#include <vector>
#include <algorithm>

#include "raylib.h"
#include "raymath.h"

#include "main.hpp"
#include "incentives.hpp"

incentives::incentives(int inputId, Vector2 inputPosition, float inputInterest, float inputDifficulty) {
    position = inputPosition;
    id = inputId;
    interest = inputInterest;
    difficulty = inputDifficulty;

    found = false;

    int neededUnitsCount = randomNum(1, 3);
    if (inputDifficulty > 5.0f) {neededUnitsCount += 1;};
    // Making n amount of random choices to decide what to put as the types needed
    while (typeOfUnitsNeeded.size() < neededUnitsCount) {
        int unitType = randomNum(0, allUnitTypes.size() - 1);
        if (std::find(typeOfUnitsNeeded.begin(), typeOfUnitsNeeded.end(), unitType) == typeOfUnitsNeeded.end()) {
            typeOfUnitsNeeded.push_back(unitType);
        }
    }

}

Vector2 incentives::getPosition() {
    return position;
}

int incentives::getID() {
    return id;
}

float incentives::getInterest() {
    return interest;
};

float incentives::getDifficulty() {
    return difficulty;
};

void incentives::draw(Vector2 mouseWorldPosition) {
    int size = (int)difficulty*5+50;

    if (found) {
        DrawRectangle(position.x - (size/2), position.y - (size/2), size, size, GREEN);
    } else {
        DrawRectangle(position.x - (size/2), position.y - (size/2), size, size, GOLD);
    }

    std::string str_converted = std::to_string(interest);
    str_converted.resize(4);
    DrawText(str_converted.c_str(), position.x - MeasureText(str_converted.c_str(), 20)/2, position.y - 7.5, 20, BLACK);

    // Display info textbox thign
    if (mouseWorldPosition.x >= position.x - size / 2 && mouseWorldPosition.x <= position.x + size / 2 &&
        mouseWorldPosition.y >= position.y - size / 2 && mouseWorldPosition.y <= position.y + size / 2) {
        std::string neededTypes;
        for (int type : typeOfUnitsNeeded) {
            neededTypes += std::to_string(type) + ", ";
        }

        int textWidth = MeasureText(neededTypes.c_str(), 20);
        DrawRectangle(position.x - textWidth / 2 - 5, position.y - size / 2 - 30, textWidth + 10, 25, BLACK);
        DrawText(neededTypes.c_str(), position.x - textWidth / 2, position.y - size / 2 - 27, 20, WHITE);
    }
}

void incentives::die() {
    found = true;
}

bool incentives::isFound() {
    return found;
}