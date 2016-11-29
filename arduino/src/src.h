#ifndef _SRC_H_
#define _SRC_H_

#include <math.h>
#include <limits.h>
#include "defines.h"
#include "motor_api.h"
#include "distance_sensor_api.h"
#include "SparkFunISL29125.h"
#include "io_api.h"

#define COLOR_COUNT 4

// Ball colors
#define GREEN_BALL 0
#define YELLOW_BALL 1
#define RED_BALL 2
#define BLUE_BALL 3
#define NOT_BALL 4
// TODO: Empty segment?

struct Segment_Queue
{
    uint8_t data[QUEUE_SIZE] = {NOT_BALL};
    uint8_t index = 0;
};

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

#endif // _SRC_H_