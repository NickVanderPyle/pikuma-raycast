#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <SDL.h>
#include "constants.h"
#include "textures.h"

const int map[MAP_NUM_ROWS][MAP_NUM_COLS] = {
        {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 0, 0, 0, 2, 2, 2, 0, 3, 0, 4, 0, 5, 0, 6, 0, 7, 0, 0, 1},
        {1, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 0, 0, 0, 2, 0, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 9, 0, 2, 0, 0, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 1},
        {1, 0, 0, 0, 4, 0, 5, 0, 6, 0, 0, 7, 0, 2, 2, 2, 2, 0, 0, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}
};

struct Player {
    float x;
    float y;
    float width;
    float height;
    int turnDirection;  // -1 for left, +1 for right
    int walkDirection;  // -1 for backwards, +1 for forwards
    float rotationAngle;
    float walkSpeed;
    float turnSpeed;
} player;

struct Ray {
    float rayAngle;
    float wallHitX;
    float wallHitY;
    float distance;
    bool wasHitVertical;
    int wallHitContent;
} rays[NUM_RAYS];

SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;
bool isGameRunning = false;
int ticksOnLastFrame = 0;

uint32_t *colorBuffer = NULL;
SDL_Texture *colorBufferTexture;

bool initializeWindow() {
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

    return true;
}

void destroyWindow() {
    freeWallTextures();
    free(colorBuffer);
    SDL_DestroyTexture(colorBufferTexture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void setup() {
    player.x = WINDOW_WIDTH / 2;
    player.y = WINDOW_HEIGHT / 2;
    player.width = 5;
    player.height = 5;
    player.turnDirection = 0;
    player.walkDirection = 0;
    player.rotationAngle = PI / 2;
    player.walkSpeed = 100;
    player.turnSpeed = 45 * (PI / 180);

    colorBuffer = (uint32_t *) malloc(sizeof(uint32_t) * (uint32_t) WINDOW_WIDTH * (uint32_t) WINDOW_HEIGHT);
    colorBufferTexture = SDL_CreateTexture(
            renderer,
            SDL_PIXELFORMAT_RGBA32,
            SDL_TEXTUREACCESS_STREAMING,
            WINDOW_WIDTH,
            WINDOW_HEIGHT
    );

    loadWallTextures();
}

bool mapHasWallAt(float x, float y) {
    if (x < 0 || x > (MAP_NUM_COLS * TILE_SIZE) || y < 0 || y > (MAP_NUM_ROWS * TILE_SIZE)) {
        return true;
    }
    int mapGridIndexX = floor(x / TILE_SIZE);
    int mapGridIndexY = floor(y / TILE_SIZE);
    return map[mapGridIndexY][mapGridIndexX] != 0;
}

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

void RenderPlayer() {
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_Rect playerRect = {
            player.x * MINIMAP_SCALE_FACTOR,
            player.y * MINIMAP_SCALE_FACTOR,
            player.width * MINIMAP_SCALE_FACTOR,
            player.height * MINIMAP_SCALE_FACTOR,
    };
    SDL_RenderFillRect(renderer, &playerRect);

    SDL_RenderDrawLine(
            renderer,
            player.x * MINIMAP_SCALE_FACTOR,
            player.y * MINIMAP_SCALE_FACTOR,
            (player.x + cos(player.rotationAngle) * 40) * MINIMAP_SCALE_FACTOR,
            (player.y + sin(player.rotationAngle) * 40) * MINIMAP_SCALE_FACTOR
    );
}

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

    int mapBoundaryX = MAP_NUM_COLS * TILE_SIZE;
    int mapBoundaryY = MAP_NUM_ROWS * TILE_SIZE;

    while (nextHorizTouchX >= 0 && nextHorizTouchX <= mapBoundaryX && nextHorizTouchY >= 0 &&
           nextHorizTouchY <= mapBoundaryY) {
        float xToCheck = nextHorizTouchX;
        float yToCheck = nextHorizTouchY + (isRayFacingUp ? -1 : 0);

        if (mapHasWallAt(xToCheck, yToCheck)) {
            //wall hit
            horizWallHitX = nextHorizTouchX;
            horizWallHitY = nextHorizTouchY;
            horizWallContent = map[(int) floor(yToCheck / TILE_SIZE)][(int) floor(xToCheck / TILE_SIZE)];
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

    while (nextVerticalTouchX >= 0 && nextVerticalTouchX <= mapBoundaryX && nextVerticalTouchY >= 0 &&
           nextVerticalTouchY <= mapBoundaryY) {
        float xToCheck = nextVerticalTouchX + (isRayFacingLeft ? -1 : 0);
        float yToCheck = nextVerticalTouchY;

        if (mapHasWallAt(xToCheck, yToCheck)) {
            //wall hit
            verticalWallHitX = nextVerticalTouchX;
            verticalWallHitY = nextVerticalTouchY;
            verticalWallContent = map[(int) floor(yToCheck / TILE_SIZE)][(int) floor(xToCheck / TILE_SIZE)];
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

void RenderMap() {
    for (int i = 0; i < MAP_NUM_ROWS; i++) {
        for (int j = 0; j < MAP_NUM_COLS; j++) {
            int tileX = j * TILE_SIZE;
            int tileY = i * TILE_SIZE;
            int tileColor = map[i][j] != 0 ? 255 : 0;
            SDL_SetRenderDrawColor(renderer, tileColor, tileColor, tileColor, 255);
            SDL_Rect mapTileRect = {
                    tileX * MINIMAP_SCALE_FACTOR,
                    tileY * MINIMAP_SCALE_FACTOR,
                    TILE_SIZE * MINIMAP_SCALE_FACTOR,
                    TILE_SIZE * MINIMAP_SCALE_FACTOR
            };
            SDL_RenderFillRect(renderer, &mapTileRect);
        }
    }
}

void renderRays() {
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

void generate3DProjection() {
    for (int x = 0; x < NUM_RAYS; x++) {
        struct Ray ray = rays[x];
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
            colorBuffer[(WINDOW_WIDTH * y) + x] = 0xFF333333;
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

            colorBuffer[(WINDOW_WIDTH * y) + x] = texelColor;
        }

        //render floor color
        for (int y = wallBottomPixel; y < WINDOW_HEIGHT; y++) {
            colorBuffer[(WINDOW_WIDTH * y) + x] = 0xFF777777;
        }
    }
}

void clearColorBuffer(uint32_t color) {
    for (int i = 0; i < WINDOW_WIDTH * WINDOW_HEIGHT; i++) {
        colorBuffer[i] = color;
    }
}

void renderColorBuffer() {
    SDL_UpdateTexture(
            colorBufferTexture,
            NULL,
            colorBuffer,
            (int) ((uint32_t) WINDOW_WIDTH * sizeof(uint32_t))
    );
    SDL_RenderCopy(renderer, colorBufferTexture, NULL, NULL);
}

void render() {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    generate3DProjection();

    //clear color buffer, displayed behind minimap
    renderColorBuffer();
    clearColorBuffer(0xFF000000);

    // minimap
    RenderMap();
    renderRays();
    RenderPlayer();

    SDL_RenderPresent(renderer);
}

int main() {
    isGameRunning = initializeWindow();

    setup();

    while (isGameRunning) {
        processInput();
        update();
        render();
    }

    destroyWindow();

    return 0;
}
