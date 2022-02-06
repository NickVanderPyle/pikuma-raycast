#include "textures.h"
#include "upng.h"
#include <stdio.h>

static const char *textureFileNames[NUM_TEXTURES] = {
        "./images/Bark.png",
        "./images/bookshelf.png",
        "./images/Bricks.png",
        "./images/Coal.png",
        "./images/Dirt.png",
        "./images/Gold0.png",
        "./images/TilledSoil.png",
        "./images/WalkStone.png",
        "./images/Stone.png",
};

void loadTextures() {
    for (int i = 0; i < NUM_TEXTURES; i++) {
        upng_t *upng;
        upng = upng_new_from_file(textureFileNames[i]);

        if (upng != NULL) {
            upng_decode(upng);
            if (upng_get_error(upng) == UPNG_EOK) {

                wallTextures[i].upngTexture = upng;
                wallTextures[i].width = upng_get_width(upng);
                wallTextures[i].height = upng_get_height(upng);
                wallTextures[i].texture_buffer = (color_t *) upng_get_buffer(upng);
            }
        }

        //upng_free(upng);
    }
}

void freeTextures() {
    for (int i = 0; i < NUM_TEXTURES; i++) {
        upng_free(wallTextures[i].upngTexture);
    }
}