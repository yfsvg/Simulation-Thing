#pragma once
#include <string>
#include "raylib.h"

class unit {
    public:
        float id;
        std::string idAsString;

        Vector2 position;

        float size;
        float direction;
        float velocity;
        float acceleration;

        Vector2 currentPositionalGoal;
        float currentDirectionalGoal;


        


        unit(float id, Vector2 position);


        void tickUpdate(float deltaTime, const std::vector<unit>& allOtherUnits);
        void draw();
        void remove();
        void cuddle();
        Vector2 getPosition();
        float getID();

        enum class unitState {
            Exploring, // Roaming behavior with randomly placed incentives and tasks. When given the task, its based on their position in the square. Each quadrant of the square gets a task inside that quadrant for ease of getting out
            Returning, // Moving towards homey
            CuddlingUp, // Close enough to the home such that it can start to find the most inner, free space inside of the square.
            Cuddling, // in their resting state
        };
        unitState currentState;

};