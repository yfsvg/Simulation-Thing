#pragma once
#include <vector>

class unit;

// New GROUOPSTATE system. Shared across all units and is global. The cycle is such:
/*
    1. Initial state hanging out, which is for units that have finished their task or units at the start of the simulation.
    2. Cuddling before exploration. Self explan.
    3. Exploration. Go to random points in their slice.
    4. Cuddling before investigation. Self explan
    5. Investigation, look at the incentives that they've discovered.
    6. Back to #2! The cycle resets.
*/
enum class groupState {
    HangingOut,
    Returning,
    CuddlingProcess,
    CuddlingBeforeExploration,
    CuddlingBeforeInvestigation,
    Exploring, // Looking around
    Investigating // Doing their assigned tasks. After their assigned tasks are finished, they return to hang out before cuddling
};

// The phase that the whole group is currently in
extern groupState groupBehavior;

extern std::vector<std::vector<bool>> cuddleGrid;

const char* groupStateToString(groupState state);

void advanceGroupState(std::vector<unit>& allUnits);
