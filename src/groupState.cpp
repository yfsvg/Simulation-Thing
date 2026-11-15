#include <math.h>
#include <cmath>
#include <vector>

#include "groupState.hpp"
#include "unit.hpp"
#include "unitMind.hpp"

std::vector<std::vector<bool>> cuddleGrid;

groupState groupBehavior = groupState::HangingOut;


const char* groupStateToString(groupState state) {
    switch (state) {
        case groupState::CuddlingBeforeExploration: return "CuddlingBeforeExploration";
        case groupState::CuddlingBeforeInvestigation: return "CuddlingBeforeInvestigation";
        case groupState::Exploring: return "Exploring";
        case groupState::Investigating: return "Investigating";
        case groupState::HangingOut: return "Hanging Out";
        case groupState::Returning: return "Returning";
        case groupState::CuddlingProcess: return "Cuddling";
        default: return "Unknown";

    }
}

// Everything that used to live inline in the KEY_X handler now lives here for centralizability and future modular use
// advanceGroupState() is the single entry point since we alreqdy know the primary order at which the group state is going in
// although maybe a setGroupState() function later? for other purposes

static bool allUnitsCuddled(const std::vector<unit>& allUnits) {
    bool returnVal = true;
    for (unit indivUnit : allUnits) {
        if (!indivUnit.hasReturnedToCuddle()) {
            returnVal = false;
        }
    }

    return returnVal;
}

static void cuddleSetup(std::vector<unit>& allUnits, groupState targetCuddleState) {
    float closestSquare = pow(std::ceil(std::sqrt((float)allUnits.size())), 2);

    cuddleGrid.clear();
    for (int row = 0; row < (int)closestSquare; row++) {
        std::vector<bool> toPush(closestSquare, false);
        cuddleGrid.push_back(toPush);
    }

    groupBehavior = targetCuddleState;

    for (unit& oneUnit : allUnits) {
        oneUnit.setGroupState(groupBehavior);
    }
}

// send ALL units outwards to  Exploring vs Investigating but right now theres actually no difference in behav
static void beginOutward(std::vector<unit>& allUnits, bool investigating) {
    groupBehavior = investigating ? groupState::Investigating : groupState::Exploring;

    for (unit& oneUnit : allUnits) {
        oneUnit.setGroupState(groupBehavior, (int)allUnits.size());
    }
}





// The single entry point: call this once per X press. determines next phase based on an order
// REFER TO GROUPSTATE.HPP LINES 6-13 FOR AN ABSTRACTED EXPLANATION OF HOW THE CYCLE WORKS
// also no invest
void advanceGroupState(std::vector<unit>& allUnits) {

    switch (groupBehavior) {
        case groupState::HangingOut:
            cuddleSetup(allUnits, groupState::CuddlingBeforeExploration);
            break;

        case groupState::CuddlingBeforeExploration:
            // Only allowed to proceed once all units are full
            if (allUnitsCuddled(allUnits)) {
                beginOutward(allUnits, /*investigating=*/false);
            }
            break;

        case groupState::Exploring:
            cuddleSetup(allUnits, groupState::CuddlingBeforeInvestigation);
            break;

        case groupState::CuddlingBeforeInvestigation:
            if (allUnitsCuddled(allUnits)) {
                assignIncentives(allUnits);
                beginOutward(allUnits, /*investigating=*/true);
            }
            break;

        case groupState::Investigating:
            cuddleSetup(allUnits, groupState::CuddlingBeforeExploration);
            break;

        default:
            break;
    }
}
