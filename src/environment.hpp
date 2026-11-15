#pragma once
#include <vector>

#include "raylib.h"

extern float mapWidth;
extern float mapHeight;

// Each tile explored will be 25 units.
extern int tileSize;
extern std::vector<std::vector<bool>> exploredTiles;

void initExploredTiles();
void markExplored(Vector2 position, int vision);

void drawBG();
