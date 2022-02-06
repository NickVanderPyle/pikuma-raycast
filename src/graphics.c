#include <SDL.h>
#include <stdint.h>
#include "graphics.h"
#include "defs.h"

static SDL_Window *window = NULL;
static SDL_Renderer *renderer = NULL;
static color_t *colorBuffer = NULL;
static SDL_Texture *colorBufferTexture;

bool initializeWindow(void) {
#ifdef __APPLE__
    SDL_SetHint(SDL_HINT_VIDEO_MAC_FULLSCREEN_SPACES, "1");
#endif // __APPLE__

    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        fprintf(stderr, "Error initializing SDL.\n");
        return false;
    }

    SDL_DisplayMode displayMode;
    SDL_GetCurrentDisplayMode(0, &displayMode);

    window = SDL_CreateWindow(
            NULL,
            SDL_WINDOWPOS_CENTERED,
            SDL_WINDOWPOS_CENTERED,
            displayMode.w,
            displayMode.h,
            SDL_WINDOW_BORDERLESS
    );
    if (!window) {
        fprintf(stderr, "Error creating SDL window.\n");
        return false;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer) {
        fprintf(stderr, "Error creating SDL renderer.\n");
        return false;
    }
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

    colorBuffer = (color_t *) malloc(sizeof(color_t) * (color_t) WINDOW_WIDTH * (color_t) WINDOW_HEIGHT);
    colorBufferTexture = SDL_CreateTexture(
            renderer,
            SDL_PIXELFORMAT_RGBA32,
            SDL_TEXTUREACCESS_STREAMING,
            WINDOW_WIDTH,
            WINDOW_HEIGHT
    );

    return true;
}

void destroyWindow(void) {
    free(colorBuffer);
    SDL_DestroyTexture(colorBufferTexture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void clearColorBuffer(color_t color) {
    for (int i = 0; i < WINDOW_WIDTH * WINDOW_HEIGHT; i++) {
        colorBuffer[i] = color;
    }
}

void renderColorBuffer(void) {
    SDL_UpdateTexture(
            colorBufferTexture,
            NULL,
            colorBuffer,
            (int) ((color_t) WINDOW_WIDTH * sizeof(color_t))
    );
    SDL_RenderCopy(renderer, colorBufferTexture, NULL, NULL);

    SDL_RenderPresent(renderer);
}

void drawPixel(int x, int y, color_t color) {
    colorBuffer[(y * WINDOW_WIDTH) + x] = color;
}

void drawRect(int x, int y, int width, int height, color_t color) {
    for (int i = x; i <= (x + width); i++) {
        for (int j = y; j <= (y + height); j++) {
            drawPixel(i, j, color);
        }
    }
}

void drawLine(int x0, int y0, int x1, int y1, color_t color) {
    int deltaX = abs(x1 - x0);
    int slopeX = x0 < x1 ? 1 : -1;
    int deltaY = -abs(y1 - y0);
    int slopeY = y0 < y1 ? 1 : -1;
    int err = deltaX + deltaY;

    while (true) {
        drawPixel(x0, y0, color);
        if (x0 == x1 && y0 == y1) {
            break;
        }
        int e2 = 2 * err;
        if (e2 >= deltaY) {
            err += deltaY;
            x0 += slopeX;
        }
        if (e2 <= deltaX) {
            err += deltaX;
            y0 += slopeY;
        }
    }
}