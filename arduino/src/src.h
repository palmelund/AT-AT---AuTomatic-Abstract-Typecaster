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
#define GARBAGE 4
// TODO: Empty segment?

// The bucket locations
#define BUCKET_COUNT 5
#define BLUE_BUCKET 0
#define GREEN_BUCKET 50
#define YELLOW_BUCKET 100
#define RED_BUCKET 260
#define GARBAGE_BUCKET 310

struct Segment
{
    bool is_occupied = false;
    uint8_t object_type;

    // NOTE: A union might be needed in the future for expanding this types
    //       functionallity.
};

struct Segment_Queue
{
    Segment data[QUEUE_SIZE] = {NOT_BALL};
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