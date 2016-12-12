#pragma once
#include <limits.h>
#include "defines.h"
#include "motor_api.h"
#include "distance_sensor_api.h"
#include "SparkFunISL29125.h"
#include "io_api.h"
#include "colors.h"
#include "segments.h"
#include "tasks.h"
#include <stdint.h>

struct RGB_Test
{
    RGB rgb1;
    RGB rgb2;
    float approx_expected;
    float approx_range;
};

void build_rgb_test(RGB_Test* t, 
    uint8_t r1, uint8_t g1, uint8_t b1, 
    uint8_t r2, uint8_t g2, uint8_t b2, 
    float e, float r);
void test_euclidean();







#define DBS_EXPECTED_DELTA 0.001