#pragma once
#include <string>
#include <vector>

#include "raylib.h"

#include "incentives.hpp"

class unit {
    public:
        float id;
        std::string idAsString;

        Vector2 position;

        float personalSpace;
        float repelStrength;

        Vector2 currentPositionalGoal;
        float currentDirectionalGoal;
        std::vector<Vector2> futurePositionalGoals;
        std::vector<float> timeInSSpentAtFutureGoal;
        bool investigatingOrNo;

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

        // Sets the unit's own currentState to match the group's phase.
        // For CuddlingBeforeExploration / CuddlingBeforeInvestigation this
        // kicks off convergence (equivalent to calling cuddle()); for
        // Exploring / Investigating this kicks off the corresponding
        // outward behavior (equivalent to calling goExplore()). This is the
        // ONLY place a unit's phase should be driven from the outside.
        // totalUnits is only used by the Exploring/Investigating cases (to
        // divide up scouting slices); pass 0 when it doesn't apply.
        void setGroupState(groupState inputGroupBehavior, int totalUnits = 0);

        void addToGoal(Vector2 goalAdd);

        void goExplore(int totalUnits, bool investigatingOrNo
            // const std::vector<int>& checkTheseOutIds // IDs into the central incentives store, passed by const ref to avoid a copy
        );

        // True once this unit has physically arrived at its cuddle spot and
        // settled back into HangingOut. Used by main.cpp to know when the
        // whole swarm has finished converging.
        bool hasReturnedToCuddle() const;

        groupState currentState;

        // basd on the one in main.hpp
        int currentType;



    private:
        float size;
        float direction;
        float velocity;
        float acceleration;
        int vision;
        bool hasArrivedAtCuddle;

        void claimCuddleSpot();
        void goExploreTarget(int totalUnits);
        void findOne(incentives incentiveFound);
};