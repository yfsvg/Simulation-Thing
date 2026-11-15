#pragma once
#include <vector>
#include "raylib.h"

#include "incentives.hpp"
#include "unit.hpp"

extern std::vector<incentives> sharedIncentivesFound;

// Dont need this, because an incentive can only be found once
// void cleanPriorityList(std::vector<incentives> allIncentivesDiscovered); // Can take repeats.

void assignIncentives(std::vector<unit>& allUnits); // Takes in shared global incentives found and assigns them

void sortByInterest();
bool anyNegative(std::vector<int> typeInventory);