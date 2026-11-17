#pragma once
#include <vector>

#include "raylib.h"

extern float mapWidth;
extern float mapHeight;

struct GameMap {
    float width = 10000;
    float height = 10000;

    int tileSize = 25;
    int cols = 0;
    int rows = 0;

    std::vector<bool> explored;

    int tileIndex(int row, int col) const { return row*cols + col; }
};

// Each tile explored will be 25 units.
void initMap(GameMap& map);
void markExplored(GameMap& map, Vector2 position, int vision);

void drawBG(const GameMap& map, const Camera2D& camera);