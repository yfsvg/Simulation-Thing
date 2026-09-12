#pragma once
#include "raylib.h"

class unit {
    public:
        int id;

        Vector2 position;
        float size;
        float direction;
        float velocity;
        float acceleration;


        unit(int id, Vector2 position);
        void tickUpdate();
        void draw();
        void remove();

};