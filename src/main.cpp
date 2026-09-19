#include <math.h>
#include <string>
#include <algorithm>
#include <vector>
#include <random>
#include <cmath>

#include "raylib.h"
#include "raymath.h"

#include "main.hpp"
#include "unit.hpp"
#include "unitMind.hpp"

std::vector<std::vector<bool>> cuddleGrid;
float mapWidth = 10000;
float mapHeight = 10000;

groupState groupBehavior = groupState::HangingOut;

std::vector<std::string> allUnitTypes = {
    "Scout",
    "Drill",
    "Imaging",
    "Climate",
    "Comm"
};

Camera2D camera = {0};
float cameraSpeed = 400.0f;

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


void initCamera() {
    camera.target = (Vector2){ 0.0f, 0.0f }; 
    camera.offset = (Vector2){ 400.0f, 400.0f };
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;
}


void drawDebugInfo(const std::vector<unit>& allUnits, const std::vector<incentives>& allIncentives) {
    static float debugScroll = 0.0f;

    const float panelY = 0.0f;
    const float panelW = 360.0f;
    const float padding = 12.0f;
    const float contentTop = panelY + 96.0f;

    auto groupStateToString = [](groupState state) {
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
    };

    debugScroll += GetMouseWheelMove() * 22.0f;
    debugScroll = std::clamp(debugScroll, 0.0f, 160.0f);

    Rectangle panelRect = { 0, panelY, panelW, (float)GetScreenHeight() };
    DrawRectangleRec(panelRect, Color{ 0, 0, 0, 200 });

    DrawText("DEBUG STATE", padding, (int)panelY + 12 - debugScroll, 22, WHITE);
    DrawText(TextFormat("Group: %s", groupStateToString(groupBehavior)), padding, (int)panelY + 40 - debugScroll, 18, RAYWHITE);
    DrawText(TextFormat("Units: %d", (int)allUnits.size()), padding, (int)panelY + 62 - debugScroll, 18, RAYWHITE);

    float y = contentTop;
    DrawText("UNITS", padding, (int)y - debugScroll, 18, RAYWHITE);
    y += 22.0f;

    for (const unit& oneUnit : allUnits) {
        std::string unitText = TextFormat("U%d [%s] %s", (int)oneUnit.id, allUnitTypes[std::clamp((int)oneUnit.currentType, 0, (int)allUnitTypes.size() - 1)].c_str(), groupStateToString(oneUnit.currentState));
        DrawText(unitText.c_str(), padding, (int)(y - debugScroll), 16, WHITE);
        y += 18.0f;

        std::string goalText = TextFormat("  Goal: (%.0f, %.0f)", oneUnit.currentPositionalGoal.x, oneUnit.currentPositionalGoal.y);
        DrawText(goalText.c_str(), padding + 5, (int)(y - debugScroll), 14, LIGHTGRAY);
        y += 16.0f;

        std::string foundText = TextFormat("  Found: %d | Assigned: %d", (int)oneUnit.incentivesFoundInSession.size(), (int)oneUnit.assignedIncentiveIds.size());
        DrawText(foundText.c_str(), padding + 5, (int)(y - debugScroll), 14, LIGHTGRAY);
        y += 18.0f;
    }
}



void cameraMovement() {
    float dt = GetFrameTime();

    // Regular offset
    if (IsKeyDown(KEY_W)) camera.target.y -= cameraSpeed * dt;
    if (IsKeyDown(KEY_S)) camera.target.y += cameraSpeed * dt;
    if (IsKeyDown(KEY_A)) camera.target.x -= cameraSpeed * dt;
    if (IsKeyDown(KEY_D)) camera.target.x += cameraSpeed * dt;

    camera.target.x = std::clamp(camera.target.x, -0.5f*mapWidth, 0.5f*mapWidth);
    camera.target.y = std::clamp(camera.target.y, -0.5f*mapHeight, 0.5f*mapHeight);

    // zooming in and out
    if (IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT)) camera.zoom += 1.0f * dt;
    if (IsKeyDown(KEY_SPACE)) camera.zoom -= 1.0f * dt;

    // Clamp zoom levels to prevent inverse/extreme values
    if (camera.zoom < 0.1f) {
        camera.zoom = 0.1f;
    }
    if (camera.zoom > 5.0f) {
        camera.zoom = 5.0f;
    }
}

void drawBG() {
    int gridSize = 50;
    Color lineColor = {150, 150, 150, 255};

    float minX = mapWidth * -0.5f;
    float maxX = mapWidth * 0.5f;
    float minY = mapHeight * -0.5f;
    float maxY = mapHeight * 0.5f;

    // horizontal lines
    for (int y = (int)minY; y <= (int)maxY; y += gridSize) {
        Vector2 startPos = { minX, (float)y };
        Vector2 endPos = { maxX, (float)y };
        DrawLineV(startPos, endPos, lineColor);
    }

    // vertical lines
    for (int x = (int)minX; x <= (int)maxX; x += gridSize) {
        Vector2 startPos = { (float)x, minY };
        Vector2 endPos = { (float)x, maxY };
        DrawLineV(startPos, endPos, lineColor);
        
    }

    // highligh already explored parts. Include fix
    for (int row = 0; row < mapHeight / 25; row++) {
        for (int col = 0; col < mapWidth / 25; col++) {

            if (!exploredTiles[row][col]) {
                DrawRectangle(col*25 - mapWidth/2, row*25 - mapHeight/2, 25, 25, {0, 0, 0, 100});
            } else {

            }

        };
    };

    // Draw center
    DrawRectangle(-10, -10, 20, 20, WHITE);
    
}


int randomNum(int min, int max) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<int> distrib(min, max);
    return distrib(gen);
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
// REFER TO MAIN.HPP LINES 13-20 FOR AN ABSTRACTED EXPLANATION OF HOW THE CYCLE WORKS
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








int main(void) {
    SetConfigFlags(FLAG_WINDOW_RESIZABLE); 
    InitWindow(1200, 800, "Simulation");
    SetTargetFPS(60);

    std::vector<unit> allUnits;
    std::vector<incentives> allIncentives;
    // Just the IDs, use the IDs as reference to get the actual data from the full incentive array
    int totalIncentives = 100;

    for (int i = 0; i < totalIncentives; i++) {
        Vector2 newPos = { (float)randomNum(mapWidth * -0.5f, mapWidth * 0.5f), (float)randomNum(mapHeight * -0.5f, mapHeight * 0.5f) };
        incentives newIncentive = incentives((int)(allIncentives.size() + 1), newPos, randomNum(0, 1000)/100.0f, randomNum(0, 1000)/100.0f);
        // Creating the incentives 
        allIncentives.push_back(newIncentive);
    }

    exploredTiles.clear();
    for (int row = 0; row < mapHeight / 25; row++) {
        std::vector<bool> toPush;
        for (int col = 0; col < mapWidth / 25; col++) {
            toPush.push_back(false);
        };
        exploredTiles.push_back(toPush);
    };


    initCamera();



    while (!WindowShouldClose()) {
        cameraMovement();

        BeginDrawing();
        ClearBackground(Color{25, 25, 25, 255});

        // Pressing C makes units, adds them to the pile.
        // You're welcome for this!
        if (IsKeyPressed(KEY_C)) {
            int inputType = 0;

            if (IsKeyDown(KEY_TWO)) {inputType = 1;}
            else if (IsKeyDown(KEY_THREE)) {inputType = 2;}
            else if (IsKeyDown(KEY_FOUR)) {inputType = 3;}
            else if (IsKeyDown(KEY_FIVE)) {inputType = 4;}

            unit newUnit = unit((float)(allUnits.size() + 1), (Vector2){0, 0}, inputType);
            newUnit.setGroupState(groupBehavior, (int)allUnits.size() + 1);
            allUnits.push_back(newUnit);
        }

        if (IsKeyPressed(KEY_X)) {
            advanceGroupState(allUnits);
        }

        BeginMode2D(camera);
            drawBG();
            Vector2 mouseWorldPosition = GetScreenToWorld2D(GetMousePosition(), camera);
            float deltaTime = GetFrameTime();
            for (unit& unitToDraw : allUnits) {
                unitToDraw.tickUpdate(deltaTime, allUnits);
                unitToDraw.draw();

                Vector2 unitPos = unitToDraw.getPosition();
                for (incentives& indivIncentive : allIncentives) {
                    // Is it within 120 pixels? If so, remove incentive
                    Vector2 incentivePos = indivIncentive.getPosition();

                    if (pow(unitPos.x - incentivePos.x, 2.0f) + pow(unitPos.y - incentivePos.y, 2) < pow(140, 2) && !indivIncentive.isFound()) {
                        indivIncentive.die();
                        // Tell the unit qbout it you found one
                        unitToDraw.incentivesFoundInSession.push_back(indivIncentive);
                    }
                }
            }

            for (incentives& indivIncentive : allIncentives) {
                indivIncentive.draw(mouseWorldPosition);
            }
        EndMode2D();

        drawDebugInfo(allUnits, allIncentives);

        EndDrawing();
    }

    CloseWindow();

    return 0;
}