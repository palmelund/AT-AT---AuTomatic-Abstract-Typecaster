#pragma once

//#ifndef _SEGMENTS_H_
//#define _SEGMENTS_H_
#include <stdint.h>
#include "colors.h"

#define QUEUE_SIZE 5                    // Size of the conveyor queue

#define KINECT_SEGMENT_INDEX 0          // TODO: The index, where the kinect is
                                        // positioned.
#define COLOR_SENSOR_SEGMENT_INDEX 3    // The index, where the color sensor
                                        // is positioned.
#define LAST_INDEX QUEUE_SIZE - 1       // The index of the last segment.

#define SEGMENT_DEGREE_LENGTH 72        // The number of degrees it takes for 
                                        // the conveyor belt motor to move one
                                        // segment.
// Object types
#define BALL 0
#define GARBAGE 1
// TODO: Empty segment?

// The bucket locations
#define BUCKET_COUNT 5
#define GARBAGE_BUCKET 0
#define GREEN_BUCKET 50
#define YELLOW_BUCKET 100
#define RED_BUCKET 260
#define BLUE_BUCKET 310

struct Segment
{
    uint8_t object_type = GARBAGE;
    uint8_t color = UNKNOWN;

    // NOTE: A union might be needed in the future for expanding this types
    //       functionallity.
};

struct Segment_Queue
{
    Segment data[QUEUE_SIZE];
    int8_t index = 0;
};

Segment* queue_next(Segment_Queue *segment_queue);
Segment* get_segment(Segment_Queue *segment_queue, uint8_t offset);

//#endif // _SEGMENTS_H_