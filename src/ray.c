#include <math.h>
#include "ray.h"
#include "player.h"
#include "map.h"
#include <float.h>

ray_t rays[NUM_RAYS];

float normalizeAngle(float angle) {
    angle = remainder(angle, TWO_PI);
    if (angle < 0) {
        angle = TWO_PI + angle;
    }
    return angle;
}

float distanceBetweenPoints(float x1, float y1, float x2, float y2) {
    return sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2));
}

void castRay(float rayAngle, int stripId) {
    rayAngle = normalizeAngle(rayAngle);
    int isRayFacingDown = rayAngle > 0 && rayAngle < PI;
    int isRayFacingUp = !isRayFacingDown;
    int isRayFacingRight = rayAngle < (0.5 * PI) || rayAngle > (1.5 * PI);
    int isRayFacingLeft = !isRayFacingRight;

    float xIntercept, yIntercept;
    float xStep, yStep;

    // Horizontal Ray Intersect Test
    bool foundHorizWallHit = false;
    float horizWallHitX = 0;
    float horizWallHitY = 0;
    int horizWallContent = 0;

    yIntercept = floor(player.y / TILE_SIZE) * TILE_SIZE;
    yIntercept += isRayFacingDown ? TILE_SIZE : 0;

    xIntercept = player.x + (yIntercept - player.y) / tan(rayAngle);

    yStep = TILE_SIZE;
    yStep *= isRayFacingUp ? -1 : 1;

    xStep = TILE_SIZE / tan(rayAngle);
    xStep *= (isRayFacingLeft && xStep > 0) ? -1 : 1;
    xStep *= (isRayFacingRight && xStep < 0) ? -1 : 1;

    float nextHorizTouchX = xIntercept;
    float nextHorizTouchY = yIntercept;

    while (isInsideMap(nextHorizTouchX, nextHorizTouchY)) {
        float xToCheck = nextHorizTouchX;
        float yToCheck = nextHorizTouchY + (isRayFacingUp ? -1 : 0);

        if (mapHasWallAt(xToCheck, yToCheck)) {
            //wall hit
            horizWallHitX = nextHorizTouchX;
            horizWallHitY = nextHorizTouchY;
            horizWallContent = getMapAt((int) floor(yToCheck / TILE_SIZE), (int) floor(xToCheck / TILE_SIZE));
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
    int verticalWallContent = 0;

    xIntercept = floor(player.x / TILE_SIZE) * TILE_SIZE;
    xIntercept += isRayFacingRight ? TILE_SIZE : 0;

    yIntercept = player.y + (xIntercept - player.x) * tan(rayAngle);

    xStep = TILE_SIZE;
    xStep *= isRayFacingLeft ? -1 : 1;

    yStep = TILE_SIZE * tan(rayAngle);
    yStep *= (isRayFacingUp && yStep > 0) ? -1 : 1;
    yStep *= (isRayFacingDown && yStep < 0) ? -1 : 1;

    float nextVerticalTouchX = xIntercept;
    float nextVerticalTouchY = yIntercept;

    while (isInsideMap(nextVerticalTouchX, nextVerticalTouchY)) {
        float xToCheck = nextVerticalTouchX + (isRayFacingLeft ? -1 : 0);
        float yToCheck = nextVerticalTouchY;

        if (mapHasWallAt(xToCheck, yToCheck)) {
            //wall hit
            verticalWallHitX = nextVerticalTouchX;
            verticalWallHitY = nextVerticalTouchY;
            verticalWallContent = getMapAt((int) floor(yToCheck / TILE_SIZE), (int) floor(xToCheck / TILE_SIZE));
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
        rays[stripId].wallHitContent = verticalWallContent;
        rays[stripId].wasHitVertical = true;
        rays[stripId].rayAngle = rayAngle;
    } else {
        rays[stripId].distance = horizontalHitDistance;
        rays[stripId].wallHitX = horizWallHitX;
        rays[stripId].wallHitY = horizWallHitY;
        rays[stripId].wallHitContent = horizWallContent;
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

void renderRays() {
    /*
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    for (int i = 0; i < NUM_RAYS; i++) {
        SDL_RenderDrawLine(
                renderer,
                player.x * MINIMAP_SCALE_FACTOR,
                player.y * MINIMAP_SCALE_FACTOR,
                rays[i].wallHitX * MINIMAP_SCALE_FACTOR,
                rays[i].wallHitY * MINIMAP_SCALE_FACTOR
        );
    }
     */
}