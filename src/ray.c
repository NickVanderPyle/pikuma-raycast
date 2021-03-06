#include <math.h>
#include "ray.h"
#include "player.h"
#include "map.h"
#include "graphics.h"
#include "utils.h"
#include <float.h>

ray_t rays[NUM_RAYS];

bool isRayFacingDown(float angle) {
    return angle > 0 && angle < PI;
}

bool isRayFacingUp(float angle) {
    return !isRayFacingDown(angle);
}

bool isRayFacingRight(float angle) {
    return angle < (0.5 * PI) || angle > (1.5 * PI);
}

bool isRayFacingLeft(float angle) {
    return !isRayFacingRight(angle);
}

void castRay(float rayAngle, int stripId) {
    normalizeAngle(&rayAngle);

    float xIntercept, yIntercept;
    float xStep, yStep;

    // Horizontal Ray Intersect Test
    bool foundHorizWallHit = false;
    float horizWallHitX = 0;
    float horizWallHitY = 0;
    int horizWallTexture = 0;

    yIntercept = floor(player.y / TILE_SIZE) * TILE_SIZE;
    yIntercept += isRayFacingDown(rayAngle) ? TILE_SIZE : 0;

    xIntercept = player.x + (yIntercept - player.y) / tan(rayAngle);

    yStep = TILE_SIZE;
    yStep *= isRayFacingUp(rayAngle) ? -1 : 1;

    xStep = TILE_SIZE / tan(rayAngle);
    xStep *= (isRayFacingLeft(rayAngle) && xStep > 0) ? -1 : 1;
    xStep *= (isRayFacingRight(rayAngle) && xStep < 0) ? -1 : 1;

    float nextHorizTouchX = xIntercept;
    float nextHorizTouchY = yIntercept;

    while (isInsideMap(nextHorizTouchX, nextHorizTouchY)) {
        float xToCheck = nextHorizTouchX;
        float yToCheck = nextHorizTouchY + (isRayFacingUp(rayAngle) ? -1 : 0);

        if (mapHasWallAt(xToCheck, yToCheck)) {
            //wall hit
            horizWallHitX = nextHorizTouchX;
            horizWallHitY = nextHorizTouchY;
            horizWallTexture = getMapAt((int) floor(yToCheck / TILE_SIZE), (int) floor(xToCheck / TILE_SIZE));
            foundHorizWallHit = true;
            break;
        }

        nextHorizTouchX += xStep;
        nextHorizTouchY += yStep;
    }

    // Vertical Ray Intersect Test
    bool foundVerticalWallHit = false;
    float verticalWallHitX = 0;
    float verticalWallHitY = 0;
    int verticalWallTexture = 0;

    xIntercept = floor(player.x / TILE_SIZE) * TILE_SIZE;
    xIntercept += isRayFacingRight(rayAngle) ? TILE_SIZE : 0;

    yIntercept = player.y + (xIntercept - player.x) * tan(rayAngle);

    xStep = TILE_SIZE;
    xStep *= isRayFacingLeft(rayAngle) ? -1 : 1;

    yStep = TILE_SIZE * tan(rayAngle);
    yStep *= (isRayFacingUp(rayAngle) && yStep > 0) ? -1 : 1;
    yStep *= (isRayFacingDown(rayAngle) && yStep < 0) ? -1 : 1;

    float nextVerticalTouchX = xIntercept;
    float nextVerticalTouchY = yIntercept;

    while (isInsideMap(nextVerticalTouchX, nextVerticalTouchY)) {
        float xToCheck = nextVerticalTouchX + (isRayFacingLeft(rayAngle) ? -1 : 0);
        float yToCheck = nextVerticalTouchY;

        if (mapHasWallAt(xToCheck, yToCheck)) {
            //wall hit
            verticalWallHitX = nextVerticalTouchX;
            verticalWallHitY = nextVerticalTouchY;
            verticalWallTexture = getMapAt((int) floor(yToCheck / TILE_SIZE), (int) floor(xToCheck / TILE_SIZE));
            foundVerticalWallHit = true;
            break;
        }

        nextVerticalTouchX += xStep;
        nextVerticalTouchY += yStep;
    }

    float horizontalHitDistance = foundHorizWallHit
                                  ? distanceBetweenPoints(player.x, player.y, horizWallHitX, horizWallHitY)
                                  : FLT_MAX;
    float verticalHitDistance = foundVerticalWallHit
                                ? distanceBetweenPoints(player.x, player.y, verticalWallHitX, verticalWallHitY)
                                : FLT_MAX;
    if (verticalHitDistance < horizontalHitDistance) {
        rays[stripId].distance = verticalHitDistance;
        rays[stripId].wallHitX = verticalWallHitX;
        rays[stripId].wallHitY = verticalWallHitY;
        rays[stripId].texture = verticalWallTexture;
        rays[stripId].wasHitVertical = true;
        rays[stripId].rayAngle = rayAngle;
    } else {
        rays[stripId].distance = horizontalHitDistance;
        rays[stripId].wallHitX = horizWallHitX;
        rays[stripId].wallHitY = horizWallHitY;
        rays[stripId].texture = horizWallTexture;
        rays[stripId].wasHitVertical = false;
        rays[stripId].rayAngle = rayAngle;
    }
}

void castAllRays() {
    for (int col = 0; col < NUM_RAYS; col++) {
        float angle = player.rotationAngle + atan((col - NUM_RAYS / 2) / DIST_PROJ_PLANE);
        castRay(angle, col);
    }
}

void renderMapRays() {
    for (int i = 0; i < NUM_RAYS; i++) {
        drawLine(
                player.x * MINIMAP_SCALE_FACTOR,
                player.y * MINIMAP_SCALE_FACTOR,
                rays[i].wallHitX * MINIMAP_SCALE_FACTOR,
                rays[i].wallHitY * MINIMAP_SCALE_FACTOR,
                0xFF0000FF
        );
    }
}