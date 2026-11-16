#pragma once
#include <string>
#include <vector>

#include "raylib.h"

extern std::vector<std::string> allUnitTypes;

class unit {
    public:
        int id;
        std::string idAsString;

        Vector2 position;

        // based on allUnitTypes, index into it
        int currentType;

        // Where the unit hangs around while it has nothing to do.
        // Player commands later on just need to move this (and the goal) somewhere else.
        Vector2 idleAnchor;
        float idleRadius;

        Vector2 currentPositionalGoal;

        float personalSpace; // radius at which repulsion kicks in
        float repelStrength; // How powerful the repulsion is

        unit(int id, Vector2 position, int typeInp);

        void tickUpdate(float deltaTime, const std::vector<unit>& allUnits);
        void draw() const;

        Vector2 getPosition() const;
        int getID() const;

    private:
        float size;
        float direction; // in degrees. Start from right, go ccw
        float currentDirectionalGoal;
        float velocity;
        float acceleration;
        int vision;

        void pickIdleGoal();
};
