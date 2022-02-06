#include "wall.h"
#include "defs.h"
#include "ray.h"
#include "player.h"
#include "graphics.h"
#include "textures.h"

void changeColorIntensity(color_t *color, float factor) {
    color_t a = *color & 0xFF000000;
    color_t r = (*color & 0x00FF0000) * factor;
    color_t g = (*color & 0x0000FF00) * factor;
    color_t b = (*color & 0x000000FF) * factor;

    *color = a | (r & 0x00FF0000) | (g & 0x0000FF00) | (b & 0x000000FF);
}

void renderWallProjection(void) {
    for (int x = 0; x < NUM_RAYS; x++) {
        ray_t ray = rays[x];
        // corrects fisheye effect
        float perpendicularDistance = ray.distance * cos(ray.rayAngle - player.rotationAngle);
        float projectedWallHeight = (TILE_SIZE / perpendicularDistance) * DIST_PROJ_PLANE;

        int wallStripHeight = projectedWallHeight;

        int wallTopPixel = (WINDOW_HEIGHT / 2) - (wallStripHeight / 2);
        wallTopPixel = wallTopPixel < 0 ? 0 : wallTopPixel;

        int wallBottomPixel = (WINDOW_HEIGHT / 2) + (wallStripHeight / 2);
        wallBottomPixel = wallBottomPixel > WINDOW_HEIGHT ? WINDOW_HEIGHT : wallBottomPixel;

        //render ceiling color
        for (int y = 0; y < wallTopPixel; y++) {
            drawPixel(x, y, 0xFF333333);
        }

        //render wall color
        int textureOffsetX = ray.wasHitVertical
                             ? (int) ray.wallHitY % TILE_SIZE
                             : (int) ray.wallHitX % TILE_SIZE;
        int texNum = ray.wallHitContent - 1;
        int textureWidth = wallTextures[texNum].width;
        int textureHeight = wallTextures[texNum].height;
        for (int y = wallTopPixel; y < wallBottomPixel; y++) {
            //set color of wall pixel from color of texture pixel.
            int distanceFromTop = y + (wallStripHeight / 2) - (WINDOW_HEIGHT / 2);
            int textureOffsetY = distanceFromTop * ((float) textureHeight / wallStripHeight);
            int textureOffset = (textureWidth * textureOffsetY) + textureOffsetX;
            color_t texelColor = wallTextures[texNum].texture_buffer[textureOffset];

            if (ray.wasHitVertical){
                changeColorIntensity(&texelColor, 0.7);
            }

            drawPixel(x, y, texelColor);
        }

        //render floor color
        for (int y = wallBottomPixel; y < WINDOW_HEIGHT; y++) {
            drawPixel(x, y, 0xFF777777);
        }
    }
}