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
    float rgb1[3];
    float rgb2[3];
    float approx_expected;
};

void build_rgb_test(RGB_Test* t, 
    float r1, float g1, float b1, 
    float r2, float g2, float b2, 
    float e);
void test_euclidean();

#define DBS_EXPECTED_DELTA 0.001