#pragma once

#include <stdint.h>
#include "defs.h"
#include "upng.h"

typedef struct {
    int width;
    int height;
    color_t *texture_buffer;
    upng_t *upngTexture;
} texture_t;

texture_t wallTextures[NUM_TEXTURES];

void loadWallTextures();

void freeWallTextures();