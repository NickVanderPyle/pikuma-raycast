#include "graphics.h"
#include <math.h>
#include "utils.h"

void normalizeAngle(float *angle) {
    *angle = remainder(*angle, TWO_PI);
    if (*angle < 0) {
        *angle = TWO_PI + *angle;
    }
}

float distanceBetweenPoints(float x1, float y1, float x2, float y2) {
    return sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2));
}