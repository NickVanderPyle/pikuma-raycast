#pragma once

typedef struct {
    float x;
    float y;
    float width;
    float height;
    int turnDirection;  // -1 for left, +1 for right
    int walkDirection;  // -1 for backwards, +1 for forwards
    float rotationAngle;
    float walkSpeed;
    float turnSpeed;
} player_t;

extern player_t player;

void movePlayer(float deltaTime);

void RenderPlayer(void);