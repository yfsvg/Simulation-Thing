#include <math.h>
#include <string>
#include <cmath>
#include <vector>
#include <algorithm>
#include <set>

#include "raylib.h"
#include "raymath.h"

#include "unitMind.hpp"

// During the exploration phase, reward bots for looking through the most empty land
// However, this reward system happens during the recongregation phase, NOT on individual execution.
// After the initial random burst (Which doesnt really matter because everything is unexplored anyways)
// The central planning system gets a list of where its eplxored (exploredTiles) and which incentives its found (sharedIncentivesFound)


// "UNCHARTED TERRITORY" IS A GLOBAL PROPERTY THAT ALL UNITS MUST KNOW!!!!

// We can simulate being under intermittent/limited connectivity as a defense for not constantly using radio.

// Need to develop a standardized time system to help with decision making in tasking robots to do tasks.
// Take into consideration the time it takes to get to the place (Distance to center, calculate as time), then mult by 2 because its a 1 way trip
// Then take into consideration the amount of time it would take to complete it before the day ends.
// First, assign the tasks with the highest interest first.
// One idea to assign the tasks would be, multiplying each interest value to a certain amount and then assigning (int)(interst) bots to the area.
// lets say the multiple is two for now.
// DO NOT make them start working on the task or whatever. As of right now, 

// Actually, scrap that. Because most tasks done by real life mars rovers aren't really parallizable, have most tasks be done by one bot
// However, some of the bots will be specialized. Specialization types: 1. Scout 2. Drilla 3. Climate analysis 4. imaging 5. Communication back to earth
// All share the same base powers so they can all do exporation work, although in the future maybe cosnider making the specialized ones less risk-taking


std::vector<incentives> sharedIncentivesFound;


void sortByInterest() {
    std::sort(sharedIncentivesFound.begin(), sharedIncentivesFound.end(), [](const incentives& a, const incentives& b) {
        if (a.interest != b.interest) return a.interest > b.interest;
        return a.difficulty < b.difficulty;
    });
}

// Checks if any of the unit types have been handed out more times than we actually have
bool anyNegative(std::vector<int> typeInventory) {
    for (int typeCount : typeInventory) {
        if (typeCount < 0) {
            return true;
        }
    }

    return false;
}

// Takes in every unit, checks inventory, then assigns it to those units
void assignIncentives(std::vector<unit>& allUnits) {
    for (unit& indivUnit : allUnits) {
        indivUnit.assignedIncentiveIds.clear();
        indivUnit.futurePositionalGoals.clear();
        indivUnit.timeInSSpentAtFutureGoal.clear();
        indivUnit.currentPositionalGoal = {0.0f, 0.0f};
    }

    // Count the units types to see what we have and what we dont have
    std::vector<int> typeInventory = {0, 0, 0, 0, 0};
    for (unit indivUnit : allUnits) {
        typeInventory[indivUnit.currentType]++;
    }
    
    // Order the incentives by interest, start greedy-assigning by the most importn
    sortByInterest();

    // Time to ASSIGN incentives!!!

    // Right now, a critical error is that even if there are multiple incentives to look for,

    std::set<unit*> globallyAssigned;

    for (int i = 0; i < sharedIncentivesFound.size(); i++) {
        const incentives& incentive = sharedIncentivesFound[i];
        bool canDo = true;
        for (int neededUnit : incentive.typeOfUnitsNeeded) {
            if (neededUnit < 0 || neededUnit >= typeInventory.size() || typeInventory[neededUnit] == 0) {
                canDo = false;
                break;
            }
        }

        if (!canDo) continue;



        std::vector<unit*> assignedUnits;
        for (int indivTypeNeeded : incentive.typeOfUnitsNeeded) {
            for (unit& candidateUnit : allUnits) {
                if (candidateUnit.currentType != indivTypeNeeded) {
                    continue;
                }


                if (globallyAssigned.count(&candidateUnit)) {
                    continue;
                }

                if (std::find(assignedUnits.begin(), assignedUnits.end(), &candidateUnit) != assignedUnits.end()) {
                    continue;
                }

                assignedUnits.push_back(&candidateUnit);
                break;
            }
        }


        for (unit* assignedUnit : assignedUnits) {
            assignedUnit -> assignedIncentiveIds.push_back(incentive.id);
            assignedUnit -> futurePositionalGoals.push_back(incentive.position);
            assignedUnit -> timeInSSpentAtFutureGoal.push_back(5.0f);
            typeInventory[assignedUnit -> currentType]--;
            assignedUnit -> currentPositionalGoal = assignedUnit->futurePositionalGoals.front();
            globallyAssigned.insert(assignedUnit);
        }
    }




}