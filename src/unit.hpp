#pragma once
#include <string>

#include "raylib.h"

#include "incentives.hpp"
extern std::vector<incentives> sharedIncentivesFound;

class unit {
    public:
        float id;
        std::string idAsString;

        Vector2 position;

        float size;
        float direction;
        float velocity;
        float acceleration;

        float personalSpace;
        float repelStrength;

        Vector2 currentPositionalGoal;
        float currentDirectionalGoal;
        bool investigatingOrNo;

        int vision;

        int explorationRadius; // Controls how far away they will look during scouting phase

        // Own memory: incentives THIS unit has personally discovered this session.
        // Kept as owned objects since they're this unit's own findings, not shared references.
        std::vector<incentives> incentivesFoundInSession;

        // Assigned targets: just IDs into the central incentive store, not copies of the data.
        // Avoids duplicating incentive data per-unit and keeps a single source of truth.
        std::vector<int> assignedIncentiveIds;
        

        unit(float id, Vector2 position, int typeInp);


        void tickUpdate(float deltaTime, const std::vector<unit>& allOtherUnits);
        void draw();
        void remove();
        void cuddle();
        Vector2 getPosition();
        float getID();

        void claimCuddleSpot();
        void goExplore(int totalUnits, bool investigatingOrNo
            // const std::vector<int>& checkTheseOutIds // IDs into the central incentives store, passed by const ref to avoid a copy
        );
        void goExploreTarget(int totalUnits);

        void findOne(incentives incentiveFound);

        enum class unitState {
            Exploring, // Edit the explroing phase such that the random positional goals are placed within a 360 / n degree slice of a circle of radius explorationRadius.
            Returning, // Moving towards homey
            CuddlingUp, // Close enough to the home such that it can start to find the most inner, free space inside of the square.
            Cuddling, // in their resting state
            HangingOut, // Staying within ~50 pixels radius of the center. Happens at the start of the simulation so they dont go running off. Their positional goal is just random points within the radius
        };
        unitState currentState;

        // basd on the one in main.hpp
        int currentType;

};