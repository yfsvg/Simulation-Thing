#include <vector>
#include <algorithm>
#include <cmath>

#include "raylib.h"

#include "environment.hpp"


void initMap(GameMap& map) {
    map.cols = (int)(map.width / map.tileSize);
    map.rows = (int)(map.height / map.tileSize);
    map.explored.assign(map.rows * map.cols, false);
}



// mark newly wandered areas as explored with a radius of vision tiles
void markExplored(GameMap& map, Vector2 position, int vision) {
    int centerCol = (int)((position.x + map.width * 0.5f) / map.tileSize);
    int centerRow = (int)((position.y + map.height * 0.5f) / map.tileSize);

    for (int row = std::max(0, centerRow - vision); row <= std::min(map.rows - 1, centerRow + vision); ++row) {
        for (int col = std::max(0, centerCol - vision); col <= std::min(map.cols - 1, centerCol + vision); ++col) {

            int dx = col - centerCol;
            int dy = row - centerRow;

            if (dx * dx + dy * dy <= vision * vision) {
                map.explored[map.tileIndex(row, col)] = true;
            }
        }
    }
}





void drawBG(const GameMap& map, const Camera2D& camera) {
    int gridSize = 50;
    Color lineColor = {150, 150, 150, 255};

    // Only draw the part of the map that's actually on screen.
    // Convert the screen corners into world space, then clamp to the map edges.
    Vector2 viewTopLeft = GetScreenToWorld2D({0, 0}, camera);
    Vector2 viewBottomRight = GetScreenToWorld2D({(float)GetScreenWidth(), (float)GetScreenHeight()}, camera);

    float minX = std::max(viewTopLeft.x, map.width * -0.5f);
    float maxX = std::min(viewBottomRight.x, map.width * 0.5f);
    float minY = std::max(viewTopLeft.y, map.height * -0.5f);
    float maxY = std::min(viewBottomRight.y, map.height * 0.5f);

    // Snap the first line down to the grid so lines don't slide around as the camera moves
    int firstGridX = (int)std::floor(minX / gridSize) * gridSize;
    int firstGridY = (int)std::floor(minY / gridSize) * gridSize;

    // horizontal lines
    for (int y = firstGridY; y <= (int)maxY; y += gridSize) {
        DrawLineV({ minX, (float)y }, { maxX, (float)y }, lineColor);
    }

    // vertical lines
    for (int x = firstGridX; x <= (int)maxX; x += gridSize) {
        DrawLineV({ (float)x, minY }, { (float)x, maxY }, lineColor);
    }

    // Darken unexplored tiles, again only the visible range of rows/columns
    int firstCol = std::max(0, (int)((minX + map.width * 0.5f) / map.tileSize));
    int lastCol = std::min(map.cols - 1, (int)((maxX + map.width * 0.5f) / map.tileSize));
    int firstRow = std::max(0, (int)((minY + map.height * 0.5f) / map.tileSize));
    int lastRow = std::min(map.rows - 1, (int)((maxY + map.height * 0.5f) / map.tileSize));

    for (int row = firstRow; row <= lastRow; row++) {
        for (int col = firstCol; col <= lastCol; col++) {
            if (!map.explored[map.tileIndex(row, col)]) {
                DrawRectangle(col * map.tileSize - map.width / 2, row * map.tileSize - map.height / 2, map.tileSize, map.tileSize, {0, 0, 0, 100});
            }
        }
    }



    // Draw center


    DrawRectangle(-10, -10, 20, 20, WHITE);


}