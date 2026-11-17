#pragma once
#include <vector>

#include "raylib.h"

#include "environment.hpp"
#include "unit.hpp"

// Shared state
struct Game {
    Camera2D camera = {};
    GameMap map;
    std::vector<unit> units;

    int nextUnitId = 1;
};