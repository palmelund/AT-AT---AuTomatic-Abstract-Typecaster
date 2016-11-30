#pragma once

#include "src.h"

void wce_task_check_first_segment(Motor* conveyor, Motor* feeder,
    Advanced_Motor* seperator, Ultra_Sound_Sensor* distance_sensor,
    uint16_t distance_to_wall, Segment_Queue* segment_queue);