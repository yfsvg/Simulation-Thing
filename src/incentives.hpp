#pragma once
#include <string>
#include "raylib.h"

class incentives {
    public:
        int id;
        std::string idAsString;
        Vector2 position;
        bool found;

        float interest; // Priority in the list after the rendevouzs meetup (0.00 to 10.00)
        float difficulty; // How tedious the task is or time it takes. Moderately correlated with interest (0.00 to 10.00)
        // During the meetup, the tasks are placed in a list by the algorithm. 
        // Incentives that have the highest interest combined with lower difficulty are prioritized the most
        // Lower interest and high difficulty are prioritized last
        // interst to difficulty ratio decides how appealing the incentive is

        // Type of units needed
        std::vector<int> typeOfUnitsNeeded;

        incentives(int id, Vector2 position, float interest, float difficulty);

        Vector2 getPosition();
        float getInterest();
        float getDifficulty();
        int getID();

        void draw(Vector2 mouseWorldPosition);
        void die();

        bool isFound();

};