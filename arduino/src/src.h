#ifndef _SRC_H_
#define _SRC_H_

#include "defines.h"
#include "motor_api.h"
#include "distance_sensor_api.h"
#include "SparkFunISL29125.h"
#include "io_api.h"

enum Ball_Color 
{
  GREEN, 
  YELLOW, 
  RED, 
  BLUE, 
  EMPTY
};

struct Segment_Queue 
{
  Ball_Color data[QUEUE_SIZE] = { EMPTY };
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