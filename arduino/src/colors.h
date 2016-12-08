#pragma once

//#ifndef _COLORS_H_
//#define _COLORS_H_

#include "SparkFunISL29125.h"
#include "defines.h"
#include <math.h>
#include <limits.h>
#include <stdint.h>

#define COLOR_COUNT 5

#define RED 0
#define GREEN 1
#define BLUE 2
#define YELLOW 3
#define UNKNOWN 4

struct RGB
{
    uint16_t red;
    uint16_t green;
    uint16_t blue;
};

struct Delta_RGB
{
    RGB rgb;
    uint16_t delta;
};

char *get_color_name(uint8_t color);
void read_color(SFE_ISL29125* RGB_sensor, RGB *result);
float euclidean_distance_3d(RGB *rgb1, RGB *rgb2);
void determin_bounding_sphere(RGB* samples, uint8_t sample_count, 
    Delta_RGB *result);
uint8_t determin_color(Delta_RGB* known_colors, RGB *color);

//#endif // _COLORS_H_
