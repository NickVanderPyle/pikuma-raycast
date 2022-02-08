#pragma once

#include <stdbool.h>

typedef struct {
    float x;
    float y;
    float distance;
    float angle;
    int texture;
    bool visible;
} sprite_t;


void renderSpriteProjection(void);

void renderMapSprites(void);
