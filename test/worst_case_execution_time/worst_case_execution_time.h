#pragma once

#include "src.h"


//#define WCE_TEST_CHECK_FIRST_SEGMENT
//#define WCE_TEST_SEND_TAKE_PICTURE
//#define WCE_TEST_DETERMIN_COLOR
//#define WCE_TEST_FEED_BALL
//#define WCE_TEST_ROTATE_SEPERATOR

#ifdef WCE_TEST_CHECK_FIRST_SEGMENT
void wce_task_check_first_segment(Motor* conveyor, Motor* feeder,
    Advanced_Motor* seperator, Ultra_Sound_Sensor* distance_sensor,
    uint16_t distance_to_wall, Segment_Queue* segment_queue);
#endif

#ifdef WCE_TEST_SEND_TAKE_PICTURE
void wce_task_send_take_picture(Motor* conveyor, Motor* feeder,
    Advanced_Motor* seperator);
#endif

#ifdef WCE_TEST_DETERMIN_COLOR
void wce_determin_color(Motor* conveyor, Motor* feeder,
    Advanced_Motor* seperator, SFE_ISL29125* color_sensor,
    Segment_Queue* segment_queue, Delta_RGB* known_colors);
#endif

#ifdef WCE_TEST_FEED_BALL
void wce_task_feed_ball(Motor* conveyor, Motor* feeder,
    Advanced_Motor* seperator);
#endif

#ifdef WCE_TEST_ROTATE_SEPERATOR
void wce_task_rotate_seperator(Motor* conveyor, Motor* feeder,
    Advanced_Motor* seperator, Segment_Queue* queue);
#endif