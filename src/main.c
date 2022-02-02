#include <stdint.h>
#include <stdbool.h>
#include <SDL.h>
#include "defs.h"
#include "textures.h"
#include "graphics.h"
#include "map.h"
#include "ray.h"
#include "player.h"

bool isGameRunning = false;
int ticksOnLastFrame = 0;

void setup() {
    loadWallTextures();
}

void processInput() {
    SDL_Event event;
    SDL_PollEvent(&event);
    switch (event.type) {
        case SDL_QUIT:
            isGameRunning = false;
            break;
        case SDL_KEYDOWN:
            switch (event.key.keysym.sym) {
                case SDLK_ESCAPE:
                    isGameRunning = false;
                    break;
                case SDLK_UP:
                    player.walkDirection = +1;
                    break;
                case SDLK_DOWN:
                    player.walkDirection = -1;
                    break;
                case SDLK_LEFT:
                    player.turnDirection = -1;
                    break;
                case SDLK_RIGHT:
                    player.turnDirection = +1;
                    break;
            }
            break;
        case SDL_KEYUP:
            switch (event.key.keysym.sym) {
                case SDLK_UP:
                    player.walkDirection = 0;
                    break;
                case SDLK_DOWN:
                    player.walkDirection = 0;
                    break;
                case SDLK_LEFT:
                    player.turnDirection = 0;
                    break;
                case SDLK_RIGHT:
                    player.turnDirection = 0;
                    break;
            }
            break;
    }
}

void update() {
    int ticksThisFrame = SDL_GetTicks();
    int timeToWait = FRAME_TIME_LENGTH - (ticksThisFrame - ticksOnLastFrame);
    if (timeToWait > 0 && timeToWait <= FRAME_TIME_LENGTH) {
        SDL_Delay(timeToWait);
    }

    float deltaTime = (ticksThisFrame - ticksOnLastFrame) / 1000.0f;
    ticksOnLastFrame = ticksThisFrame;

    movePlayer(deltaTime);
    castAllRays();
}

void renderWallProjection() {
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
            uint32_t texelColor = wallTextures[texNum].texture_buffer[textureOffset];

            drawPixel(x, y, texelColor);
        }

        //render floor color
        for (int y = wallBottomPixel; y < WINDOW_HEIGHT; y++) {
            drawPixel(x, y, 0xFF777777);
        }
    }
}

void render() {
    clearColorBuffer(0xFF000000);

    renderWallProjection();

    // minimap
    RenderMap();
    renderRays();
    RenderPlayer();

    renderColorBuffer();
}

void releaseResources() {
    freeWallTextures();
    destroyWindow();
}

int main() {
    isGameRunning = initializeWindow();

    setup();

    while (isGameRunning) {
        processInput();
        update();
        render();
    }

    releaseResources();

    return 0;
}
