#include <string>
#include <algorithm>
#include <vector>

#include "raylib.h"

#include "groupState.hpp"
#include "unit.hpp"
#include "unitMind.hpp"
#include "debug.hpp"





void drawDebugInfo(const std::vector<unit>& allUnits, const std::vector<incentives>& allIncentives) {
    static float debugScroll = 0.0f;

    const float panelY = 0.0f;
    const float panelW = 360.0f;
    const float padding = 12.0f;
    const float contentTop = panelY + 96.0f;

    debugScroll += GetMouseWheelMove() * 22.0f;
    debugScroll = std::clamp(debugScroll, 0.0f, 4000.0f);

    Rectangle panelRect = { 0, panelY, panelW, (float)GetScreenHeight() };
    DrawRectangleRec(panelRect, Color{ 0, 0, 0, 200 });

    DrawText("DEBUG STATE", padding, (int)panelY + 12 - debugScroll, 22, WHITE);
    DrawText(TextFormat("Group: %s", groupStateToString(groupBehavior)), padding, (int)panelY + 40 - debugScroll, 18, RAYWHITE);
    DrawText(TextFormat("Units: %d", (int)allUnits.size()), padding, (int)panelY + 62 - debugScroll, 18, RAYWHITE);

    float y = contentTop;
    y += 15.0f;
    DrawText("UNITS", padding, (int)y - debugScroll, 18, RAYWHITE);
    y += 22.0f;

    for (const unit& oneUnit : allUnits) {
        std::string unitText = TextFormat("U%d [%s] %s", (int)oneUnit.id, allUnitTypes[std::clamp((int)oneUnit.currentType, 0, (int)allUnitTypes.size() - 1)].c_str(), groupStateToString(oneUnit.currentState));
        DrawText(unitText.c_str(), padding, (int)(y - debugScroll), 16, WHITE);
        y += 18.0f;

        std::string goalText = TextFormat("Goal: (%.0f, %.0f). Doing task: %i", oneUnit.currentPositionalGoal.x, oneUnit.currentPositionalGoal.y, (oneUnit.doingTask) ? 1 : 0);
        DrawText(goalText.c_str(), padding + 15, (int)(y - debugScroll), 14, LIGHTGRAY);
        y += 16.0f;

        std::string foundText = TextFormat("Found: %d, Assigned: %d", (int)oneUnit.incentivesFoundInSession.size(), (int)oneUnit.assignedIncentiveIds.size());
        DrawText(foundText.c_str(), padding + 15, (int)(y - debugScroll), 14, LIGHTGRAY);
        y += 18.0f;

        if (oneUnit.assignedIncentiveIds.size() > 0) {
            foundText = TextFormat("Future priority: %i", (int)oneUnit.assignedIncentiveIds[0]);
            DrawText(foundText.c_str(), padding + 15, (int)(y - debugScroll), 14, LIGHTGRAY);
            y += 18.0f;
        }
    }



    y += 20.0f;
    DrawText("SHARED INCENTIVE LIST", padding, (int)y - debugScroll, 18, RAYWHITE);
    y += 22.0f;
    int i = 0;
    for (const incentives& sharedIncentiveIndiv : sharedIncentivesFound) {
        i++;
        std::string incentiveText = TextFormat("Incentive #%i", i);
        DrawText(incentiveText.c_str(), padding, (int)(y - debugScroll), 16, WHITE);
        y += 18.0f;

        incentiveText = TextFormat("Interest Level: %f", sharedIncentiveIndiv.interest);
        DrawText(incentiveText.c_str(), padding + 15, (int)(y - debugScroll), 14, LIGHTGRAY);
        y += 18.0f;

        incentiveText = TextFormat("Difficulty Level: %f", sharedIncentiveIndiv.difficulty);
        DrawText(incentiveText.c_str(), padding + 15, (int)(y - debugScroll), 14, LIGHTGRAY);
        y += 18.0f;


    }

}
