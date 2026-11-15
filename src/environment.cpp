// There will be a constant risk of breakdown, an incredibly small chance for each bot every few ticks.
// Going through slopes or rough environment will speed up the breakdown process by just increasing risk to simulate bots falling over and stuff


// Certain areas dicated by perlin noise will be sloped upwards or downwards. 
// There will be a separate map that determines the rockiness and sandiness of certain areas like minecraft biomes, each coming with their own risks.


#include <vector>
#include <algorithm>

#include "raylib.h"

#include "environment.hpp"

float mapWidth = 10000;
float mapHeight = 10000;

int tileSize = 25;
std::vector<std::vector<bool>> exploredTiles;


void initExploredtiles() {
    exploredTiles.clear();
    for (int row = 0; row < mapHeight / tileSize; row++) {
        std::vector<bool> toPush;
        for (int col = 0; col < mapWidth / tileSize; col++) {
            toPush.push_back(false);
        };
        exploredTiles.push_back(toPush);
    };
}


// mark newly wandered areas as explored with a radius of vision tiles
void markExplored(Vector2 position, int vision) {
    int centerVertical = (int)((position.x + mapWidth * 0.5f) / tileSize);
    int centerHorizontal = (int)((position.y + mapHeight * 0.5f) / tileSize);

    for (int row = std::max(0, centerHorizontal - vision); row <= std::min((int)(mapHeight / tileSize) - 1, centerHorizontal + vision); ++row) {
        for (int col = std::max(0, centerVertical - vision); col <= std::min((int)(mapWidth / tileSize) - 1, centerVertical + vision); ++col) {

            int dx = col - centerVertical;
            int dy = row - centerHorizontal;

            if (dx * dx + dy * dy <= vision * vision) {
                exploredTiles[row][col] = true;
            }
        }
    }
}






void drawBG() {
        int gridSize = 50;
    Color lineColor = {150, 150, 150, 255};

    float minX = mapWidth * -0.5f;
    float maxX = mapWidth * 0.5f;
    float minY = mapHeight * -0.5f;
    float maxY = mapHeight * 0.5f;

    // horizontal lines
    for (int y = (int)minY; y <= (int)maxY; y += gridSize) {
        Vector2 startPos = { minX, (float)y };
        Vector2 endPos = { maxX, (float)y };
        DrawLineV(startPos, endPos, lineColor);
    }


    // vertical lines
    for (int x = (int)minX; x <= (int)maxX; x += gridSize) {
        Vector2 startPos = { (float)x, minY };
        Vector2 endPos = { (float)x, maxY };
        DrawLineV(startPos, endPos, lineColor);

    }

    // hightlight already explored parts. Include fix.
    for (int row = 0; row < mapHeight / tileSize; row++) {
        for (int col = 0; col < mapWidth / tileSize; col++) {

            if (!exploredTiles[row][col]) {
                DrawRectangle(col*tileSize - mapWidth/2, row*tileSize - mapHeight/2, tileSize, tileSize, {0, 0, 0, 100});
            } else {

            }

        };
    };

    // Draw center
    DrawRectangle(-10, -10, 20, 20, WHITE);

}



