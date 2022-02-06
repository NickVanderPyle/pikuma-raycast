#include "player.h"
#include "defs.h"
#include "map.h"
#include "graphics.h"

player_t player = {
        .x = WINDOW_WIDTH / 2,
        .y = WINDOW_HEIGHT / 2,
        .width = 5,
        .height = 5,
        .turnDirection = 0,
        .walkDirection = 0,
        .rotationAngle = PI / 2,
        .walkSpeed = 100,
        .turnSpeed = 45 * (PI / 180),
};

void movePlayer(float deltaTime) {
    player.rotationAngle += player.turnDirection * player.turnSpeed * deltaTime;

    float moveStep = player.walkDirection * player.walkSpeed * deltaTime;
    float newPlayerX = player.x + cos(player.rotationAngle) * moveStep;
    float newPlayerY = player.y + sin(player.rotationAngle) * moveStep;

    if (!mapHasWallAt(newPlayerX, newPlayerY)) {
        player.x = newPlayerX;
        player.y = newPlayerY;
    }
}

void RenderMapPlayer(void) {
    drawRect(
            player.x * MINIMAP_SCALE_FACTOR,
            player.y * MINIMAP_SCALE_FACTOR,
            player.width * MINIMAP_SCALE_FACTOR,
            player.height * MINIMAP_SCALE_FACTOR,
            0xFFFFFFFF
    );
}