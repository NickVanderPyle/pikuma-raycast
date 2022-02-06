#pragma once

#include <stdbool.h>
#include <stdint.h>

bool initializeWindow(void);

void destroyWindow(void);

void clearColorBuffer(uint32_t color);

void renderColorBuffer(void);

void drawPixel(int x, int y, uint32_t color);

void drawRect(int x, int y, int width, int height, uint32_t color);

void drawLine(int x0, int y0, int x1, int y1, uint32_t color);