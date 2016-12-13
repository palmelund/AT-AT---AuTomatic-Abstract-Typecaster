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

#define SENSOR_PINGS 40
#define FEEDER_ITERATION 4              // The iterations between each feed
#define FEEDER_DEGREES 90               // The degrees the feeder needs to turn
                                        // to feed a ball.
#define FEEDER_MAX_DEGREES 360
#define FEEDER_START_DEGREES 0
                                        
int32_t task_calibrate_ultra_sound_sensor(Ultra_Sound_Sensor* distance_sensor);

void task_calibrate_colors(int32_t* conveyor_target, Motor* conveyor, 
    SFE_ISL29125* rgb_sensor, Delta_RGB colors[COLOR_COUNT]);

void task_check_first_segment(Segment_Queue* segment_queue);

void task_determin_color(SFE_ISL29125* color_sensor,
    Segment_Queue* segment_queue, Delta_RGB* known_colors);

void task_feed_ball(Motor* feeder);

void task_rotate_seperator(Advanced_Motor* separator, Segment_Queue* queue);




//void task_send_take_picture(Segment_Queue* queue);
//void task_request_object_info(Segment_Queue* segment_queue);

//#endif // _TASKS_H_