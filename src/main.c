#include <stdint.h>
#include <stdbool.h>
#include <SDL.h>
#include "defs.h"
#include "textures.h"
#include "graphics.h"
#include "map.h"
#include "ray.h"
#include "player.h"
#include "wall.h"
#include "sprite.h"

bool isGameRunning = false;
int ticksOnLastFrame = 0;

void setup() {
    loadTextures();
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

void render() {
    clearColorBuffer(0xFF000000);

    renderWallProjection();
    renderSpriteProjection();

    // minimap
    RenderMapGrid();
    renderMapRays();
    renderMapSprites();
    RenderMapPlayer();

    renderColorBuffer();
}

void releaseResources() {
    freeTextures();
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

    return EXIT_SUCCESS;
}
