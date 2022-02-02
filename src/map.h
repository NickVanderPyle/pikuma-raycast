#pragma once

#include <stdbool.h>

#define MAP_NUM_ROWS 13
#define MAP_NUM_COLS 20

bool mapHasWallAt(float x, float y);

void RenderMap();

int getMapAt(int i, int j);

bool isInsideMap(float x, float y);