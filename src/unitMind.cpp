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
        return a.interest < b.interest;
    });
}

// Takes in every unit, checks inventory, then assigns it to those units
void assignIncentives(std::vector<unit>& allUnits) {
    // Count the units types to see what we have and what we dont have
    std::vector<int> typeInventory = {0, 0, 0, 0, 0};
    for (unit indivUnit : allUnits) {
        typeInventory[indivUnit.currentType]++;
    }
    
    // Order the incentives by interest, start greedy-assigning by the most importn
    sortByInterest();

    // One entry per incentive: can we currently field every unit type it needs?
    std::vector<bool> canDo(sharedIncentivesFound.size(), true);
    for (int i = 0; i < sharedIncentivesFound.size(); i++) {
        for (int neededUnit : sharedIncentivesFound[i].typeOfUnitsNeeded) {
            if (typeInventory[neededUnit] == 0) {
                canDo[i] = false;
                break;
            }
        }
    }

    // Time to ASSIGN incentives!!!

    for (int i = 0; i < sharedIncentivesFound.size(); i++) {
        if (!canDo[i]) {
            continue;
        }

        // Loop through all needed ones, try to find one needed and then assign it. Assign meaning give it to their list
        for (int indivTypeNeeded : sharedIncentivesFound[i].typeOfUnitsNeeded) {
            for (unit& candidateUnit : allUnits) {
                if (candidateUnit.currentType != indivTypeNeeded) {
                    continue;
                }

                bool alreadyAssigned = std::find(
                    candidateUnit.assignedIncentiveIds.begin(),
                    candidateUnit.assignedIncentiveIds.end(),
                    sharedIncentivesFound[i].id
                ) != candidateUnit.assignedIncentiveIds.end();

                if (alreadyAssigned) {
                    continue;
                }

                candidateUnit.assignedIncentiveIds.push_back(sharedIncentivesFound[i].id);
                typeInventory[indivTypeNeeded]--;
                break;
            }
        }
    }
}