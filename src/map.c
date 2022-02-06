#include <stdint.h>
#include "map.h"
#include "defs.h"
#include "graphics.h"

static const int map[MAP_NUM_ROWS][MAP_NUM_COLS] = {
        {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 0, 0, 0, 2, 2, 2, 0, 3, 0, 4, 0, 5, 0, 6, 0, 7, 0, 0, 1},
        {1, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 0, 0, 0, 2, 0, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 9, 0, 2, 0, 0, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 1},
        {1, 0, 0, 0, 4, 0, 5, 0, 6, 0, 0, 7, 0, 2, 2, 2, 2, 0, 0, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}
};

bool mapHasWallAt(float x, float y) {
    if (x < 0 || x > (MAP_NUM_COLS * TILE_SIZE) || y < 0 || y > (MAP_NUM_ROWS * TILE_SIZE)) {
        return true;
    }
    int mapGridIndexX = floor(x / TILE_SIZE);
    int mapGridIndexY = floor(y / TILE_SIZE);
    return map[mapGridIndexY][mapGridIndexX] != 0;
}

int getMapAt(int i, int j) {
    return map[i][j];
}

void RenderMapGrid() {
    for (int i = 0; i < MAP_NUM_ROWS; i++) {
        for (int j = 0; j < MAP_NUM_COLS; j++) {
            int tileX = j * TILE_SIZE;
            int tileY = i * TILE_SIZE;
            color_t tileColor = map[i][j] != 0 ? 0XFFFFFFFF : 0x00000000;
            drawRect(tileX * MINIMAP_SCALE_FACTOR,
                     tileY * MINIMAP_SCALE_FACTOR,
                     TILE_SIZE * MINIMAP_SCALE_FACTOR,
                     TILE_SIZE * MINIMAP_SCALE_FACTOR,
                     tileColor);
        }
    }
}

bool isInsideMap(float x, float y) {
    int mapBoundaryX = MAP_NUM_COLS * TILE_SIZE;
    int mapBoundaryY = MAP_NUM_ROWS * TILE_SIZE;
    return x >= 0 && x <= mapBoundaryX && y >= 0 && y <= mapBoundaryY;
}