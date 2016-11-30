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

void wce_task_send_take_picture(Motor* conveyor, Motor* feeder,
    Advanced_Motor* seperator)
{
    motor_turn(conveyor);
    motor_turn(feeder);
    advanced_motor_turn(seperator, FORWARD);

    delay(1000);

    uint32_t time_start, time_end;
    for (uint8_t i = 0; i < CALIBRACTION_ITERATIONS; ++i)
    {
        time_start = micros();

        task_send_take_picture();

        time_end = micros();

        Serial.println("");
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

void wce_determin_color(Motor* conveyor, Motor* feeder,
    Advanced_Motor* seperator, SFE_ISL29125* color_sensor,
    Segment_Queue* segment_queue, Delta_RGB* known_colors)
{
    motor_turn(conveyor);
    motor_turn(feeder);
    advanced_motor_turn(seperator, FORWARD);

    for (uint8_t i = 0; i < QUEUE_SIZE; ++i)
    {
        Segment* segment = get_segment(segment_queue, i);
        segment->object_type = BALL;
        segment->is_occupied = true;
    }

    delay(1000);

    uint32_t time_start, time_end;
    for (uint8_t i = 0; i < CALIBRACTION_ITERATIONS; ++i)
    {

        time_start = micros();

        task_determin_color(color_sensor, segment_queue, known_colors);

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

void wce_task_feed_ball(Motor* conveyor, Motor* feeder,
    Advanced_Motor* seperator)
{
    motor_turn(conveyor);
    motor_turn(feeder);

    delay(1000);

    uint32_t time_start, time_end;
    for (uint8_t i = 0; i < CALIBRACTION_ITERATIONS; ++i)
    {
        time_start = micros();

        task_feed_ball(feeder);

        time_end = micros();

        Serial.print(i);
        Serial.print("\t& ");
        Serial.print(time_end - time_start);
        Serial.println("\t\\\\ \\hline");

        delay(1000);
    }

    motor_stop(conveyor);
    motor_stop(feeder);
}

//#define YELLOW_BUCKET 100
//#define RED_BUCKET 260
void wce_task_rotate_seperator(Motor* conveyor, Motor* feeder,
    Advanced_Motor* seperator, Segment_Queue* queue)
{
    motor_turn(conveyor);
    advanced_motor_turn(seperator, FORWARD);

    Segment* segment = get_segment(queue, LAST_INDEX);
    segment->is_occupied = true;
    segment->object_type = BALL;
    segment->color = YELLOW;

    task_rotate_seperator(seperator, queue);

    delay(1000);

    uint32_t time_start, time_end;
    for (uint8_t i = 0; i < CALIBRACTION_ITERATIONS; ++i)
    {
        segment->color = i % 2 == 0 ? RED : YELLOW;

        time_start = micros();

        task_rotate_seperator(seperator, queue);

        time_end = micros();

        Serial.print(i);
        Serial.print("\t& ");
        Serial.print(time_end - time_start);
        Serial.println("\t\\\\ \\hline");

        delay(1000);
    }

    motor_stop(conveyor);
    advanced_motor_stop(seperator);
}