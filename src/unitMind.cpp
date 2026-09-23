#include <math.h>
#include <string>
#include <cmath>
#include <vector>
#include <algorithm>
#include <set>

#include "raylib.h"
#include "raymath.h"

#include "main.hpp"
#include "unitMind.hpp"


std::vector<incentives> sharedIncentivesFound;
std::vector<std::vector<bool>> exploredTiles;


void sortByInterest() {
    std::sort(sharedIncentivesFound.begin(), sharedIncentivesFound.end(), [](const incentives& a, const incentives& b) {
        if (a.interest != b.interest) return a.interest > b.interest;
        return a.difficulty < b.difficulty;
    });
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