#pragma once


//#ifndef _TASKS_H_
//#define _TASKS_H_

#include "distance_sensor_api.h"
#include "io_api.h"
#include "SparkFunISL29125.h"
#include "colors.h"
#include "segments.h"
#include "motor_api.h"
#include <stdint.h>

#define FEEDER_ITERATION 8              // The iterations between each feed
#define FEEDER_DEGREES 90               // The degrees the feeder needs to turn
                                        // to feed a ball.
                                        
int32_t task_calibrate_ultra_sound_sensor(Ultra_Sound_Sensor* distance_sensor);


void task_check_first_segment(Ultra_Sound_Sensor* distance_sensor, 
    uint16_t distance_to_wall, Segment_Queue* segment_queue);

void task_send_take_picture();

void task_determin_color(SFE_ISL29125* color_sensor, 
    uint16_t distance_to_wall, Segment_Queue* segment_queue);
void task_request_object_info(Segment_Queue* segment_queue);

void task_feed_ball(Motor* feeder);

void task_rotate_seperator(Advanced_Motor* separator, Segment_Queue* queue);

//#endif // _TASKS_H_