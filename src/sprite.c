#include "sprite.h"
#include "graphics.h"
#include "player.h"
#include "utils.h"
#include "upng.h"
#include "textures.h"
#include "ray.h"

#define NUM_SPRITES 4

static sprite_t sprites[NUM_SPRITES] = {
        {.x=640, .y=630, .texture = 9},
        {.x=450, .y=600, .texture = 10},
        {.x=300, .y=400, .texture = 11},
        {.x=90, .y=100, .texture = 12},
};

void renderSpriteProjection(void) {
    sprite_t visibleSprites[NUM_SPRITES];
    int numVisibleSprites = 0;

    for (int i = 0; i < NUM_SPRITES; i++) {
        sprite_t *sprite = &sprites[i];
        float angleSpritePlayer = player.rotationAngle - atan2(sprite->y - player.y, sprite->x - player.x);

        if (angleSpritePlayer > PI) {
            angleSpritePlayer -= TWO_PI;
        }
        if (angleSpritePlayer < -PI) {
            angleSpritePlayer += TWO_PI;
        }
        angleSpritePlayer = fabs(angleSpritePlayer);

        const float EPSILON = 0.2;
        if (angleSpritePlayer < (FOV_ANGLE / 2) + EPSILON) {
            sprite->visible = true;
            sprite->angle = angleSpritePlayer;
            sprite->distance = distanceBetweenPoints(
                    sprite->x, sprite->y,
                    player.x, player.y);

            visibleSprites[numVisibleSprites] = *sprite;
            numVisibleSprites++;
        } else {
            sprite->visible = false;
        }
    }

    for (int i = 0; i < numVisibleSprites - 1; i++) {
        for (int j = i + 1; j < numVisibleSprites; j++) {
            if (visibleSprites[i].distance < visibleSprites[j].distance) {
                sprite_t temp = visibleSprites[i];
                visibleSprites[i] = visibleSprites[j];
                visibleSprites[j] = temp;
            }
        }
    }

    for (int i = 0; i < numVisibleSprites; i++) {
        sprite_t sprite = visibleSprites[i];

        float perpendicularDistance = sprite.distance * cos(sprite.angle);

        float spriteHeight = (TILE_SIZE / perpendicularDistance) * DIST_PROJ_PLANE;
        float spriteWidth = spriteHeight;

        float spriteTopY = (WINDOW_HEIGHT / 2) - (spriteHeight / 2);
        spriteTopY = spriteTopY < 0 ? 0 : spriteTopY;
        float spriteBottomY = (WINDOW_HEIGHT / 2) + (spriteHeight / 2);
        spriteBottomY = spriteBottomY > WINDOW_HEIGHT ? 0 : spriteBottomY;

        float spriteAngle = atan2(sprite.y - player.y, sprite.x - player.x) - player.rotationAngle;
        float spriteScreenPosX = tan(spriteAngle) * DIST_PROJ_PLANE;

        float spriteLeftX = (WINDOW_WIDTH / 2) + spriteScreenPosX - (spriteWidth / 2);
        float spriteRightX = spriteLeftX + spriteWidth;

        int textureWidth = upng_get_width(textures[sprite.texture]);
        int textureHeight = upng_get_height(textures[sprite.texture]);

        for (int x = spriteLeftX; x < spriteRightX; x++) {
            float texelWidth = textureWidth / spriteWidth;
            int textureOffsetX = (x - spriteLeftX) * texelWidth;

            for (int y = spriteTopY; y < spriteBottomY; y++) {
                if (x > 0 && x < WINDOW_WIDTH && y > 0 && y < WINDOW_HEIGHT) {
                    int distanceFromTop = y + (spriteHeight / 2) - (WINDOW_HEIGHT / 2);
                    int textureOffsetY = distanceFromTop * (textureHeight / spriteHeight);

                    color_t *spriteTextureBuffer = (color_t *) upng_get_buffer(textures[sprite.texture]);
                    color_t texelColor = spriteTextureBuffer[(textureWidth * textureOffsetY) + textureOffsetX];

                    if (sprite.distance < rays[x].distance && texelColor != 0xFFFF00FF) {
                        drawPixel(x, y, texelColor);
                    }
                }
            }
        }
    }
}

void renderMapSprites(void) {
    for (int i = 0; i < NUM_SPRITES; i++) {
        sprite_t sprite = sprites[i];
        color_t color = sprite.visible ? 0xFF00FFFF : 0xFF444444;
        drawRect(
                sprite.x * MINIMAP_SCALE_FACTOR,
                sprite.y * MINIMAP_SCALE_FACTOR,
                5,
                5,
                color);
    }
}