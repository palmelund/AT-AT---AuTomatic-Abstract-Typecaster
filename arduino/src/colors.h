#pragma once

//#ifndef _COLORS_H_
//#define _COLORS_H_

#include "SparkFunISL29125.h"
#include "defines.h"
#include <math.h>
#include <limits.h>
#include <stdint.h>

#define COLOR_COUNT 4

#define RED 0x00
#define GREEN 0x01
#define BLUE 0x02
#define YELLOW 0x03
#define UNKNOWN 0x04

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
float euclidean_distance_3d(
    float x1, float y1, float z1,
    float x2, float y2, float z2);
void determin_bounding_sphere(RGB* samples, uint8_t sample_count, 
    Delta_RGB *result);
uint8_t determin_color(Delta_RGB* known_colors, RGB *color);

//#endif // _COLORS_H_
