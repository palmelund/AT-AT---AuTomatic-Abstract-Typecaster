#include "worst_case_execution_time.h"

void wce_task_check_first_segment(Motor* conveyor, Motor* feeder,
    Advanced_Motor* seperator, Ultra_Sound_Sensor* distance_sensor,
    uint16_t distance_to_wall, Segment_Queue* segment_queue)
{
    motor_turn(conveyor);
    motor_turn(feeder);
    advanced_motor_turn(seperator, FORWARD);

    delay(1000);

    uint32_t time_start, time_end;
    for (uint8_t i = 0; i < CALIBRACTION_ITERATIONS; ++i)
    {
        time_start = micros();
        task_check_first_segment(distance_sensor, distance_to_wall, 
            segment_queue);
        time_end = micros();

        Serial.print(i);
        Serial.print("\t& ");
        Serial.print(time_end - time_start);
        Serial.println("\t\\\\ \\hline");

        delay(100);
    }

    motor_stop(conveyor);
    motor_stop(feeder);
    advanced_motor_stop(seperator);
}