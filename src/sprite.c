#include "sprite.h"
#include "graphics.h"
#include "player.h"

#define NUM_SPRITES 1

static sprite_t sprites[NUM_SPRITES] = {
        {.x=640, .y=630, .texture = 9}
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

        if (angleSpritePlayer < (FOV_ANGLE / 2)) {
            sprite->visible = true;
            visibleSprites[numVisibleSprites] = *sprite;
            numVisibleSprites++;
        } else {
            sprite->visible = false;
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