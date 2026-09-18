#pragma once
#include <vector>
#include "raylib.h"
#include "unit.hpp"

extern std::vector<incentives> sharedIncentivesFound;

// Each tile explored will be 25 units. 
extern std::vector<std::vector<bool>> exploredTiles;

// Dont need this, because an incentive can only be found once
// void cleanPriorityList(std::vector<incentives> allIncentivesDiscovered); // Can take repeats.

void assignIncentives(std::vector<unit>& allUnits); // Takes in shared global incentives found and assigns them

void sortByInterest();
bool anyNegative(std::vector<int> typeInventory);