#include "tasks.h"

// -------------------------------
// Setup tasks
// -------------------------------
int32_t task_calibrate_ultra_sound_sensor(Ultra_Sound_Sensor *distance_sensor)
{
    int32_t min = distance_sensor_measure_distance(distance_sensor);
    int32_t current;
    for (uint8_t i = 0; i < CALIBRACTION_ITERATIONS - 1; ++i)
    {
        // We delay a little, because it seems that calling measure_distance
        // to rapidly, affects the results.
        delay(5);

        //DEBUG_PRINTLN_VAR(min);
        current = distance_sensor_measure_distance(distance_sensor);

        if (current < min)
            min = current;
    }

    return min - 8;
}

// -------------------------------
// Task that are executed by the cyclic executive
// -------------------------------
void task_check_first_segment(Segment_Queue *segment_queue)
{
    Segment *first_segment = queue_next(segment_queue);

    // Tell the computer to take a picture
    Message take_picture_message;
    take_picture_message.type = MESSAGE_TYPE_COMMAND;
    take_picture_message.command.type = MESSAGE_COMMAND_TAKE_PICURE;

    io_send_message(&take_picture_message);


    // TODO: Computer will probably send both color and shape. Make it work!
    Message response;
    io_await_message(&response);

    first_segment->object_type = response.object.type;
    first_segment->color = response.object.color;


/*
    Segment *first_segment = queue_next(segment_queue);

    // DEBUG
    first_segment->is_occupied = true;
    first_segment->object_type = BALL;
    return;

    for (uint8_t i = 0; i < SENSOR_PINGS; ++i)
    {
        int32_t test_dist = distance_sensor_measure_distance(distance_sensor);

        // Tests if a ball is in front of sensor
        if (test_dist < distance_to_wall)
        {
            DEBUG_PRINTLN("Segment was occupied");
            first_segment->is_occupied = true;
            first_segment->object_type = BALL;
            first_segment->color = UNKNOWN;
            break;
        }
        else
        {
            first_segment->is_occupied = false;
        }
    }
*/
}

void task_determin_color(SFE_ISL29125 *color_sensor,
                         Segment_Queue *segment_queue, Delta_RGB *known_colors)
{
    Segment *segment = get_segment(segment_queue, COLOR_SENSOR_SEGMENT_INDEX);

    if (segment->is_occupied && segment->object_type == BALL)
    {
        uint8_t results[COLOR_COUNT] = { 0 };

        for (uint8_t i = 0; i < SENSOR_PINGS; ++i)
        {
            RGB color;
            read_color(color_sensor, &color);
            results[determin_color(known_colors, &color)] += 1;
        }

        uint8_t determined_color;
        int8_t max = -1;
        for (uint8_t i = 0; i < COLOR_COUNT; ++i)
        {
            if (results[i] > max)
            {
                max = results[i];
                determined_color = i;
            }
        }

        segment->color = determined_color;

        DEBUG_PRINT("C: ");
        DEBUG_PRINTLN(get_color_name(determined_color));
    }
}

void task_feed_ball(Motor *feeder)
{
    static uint8_t feed_counter = FEEDER_ITERATION;
    static int16_t deg = FEEDER_START_DEGREES + FEEDER_DEGREES;

    // We only feed a ball every x iterations
    if (feed_counter == FEEDER_ITERATION)
    {
        motor_turn_to_degree(feeder, deg);

        deg += FEEDER_DEGREES;
        if (deg == FEEDER_MAX_DEGREES)
            deg = FEEDER_START_DEGREES;

        feed_counter = 0;
    }

    feed_counter++;
}

void task_rotate_seperator(Advanced_Motor *separator, Segment_Queue *queue)
{
    static uint8_t last_position = GARBAGE_BUCKET;
    static int16_t bucket_position[BUCKET_COUNT] = {
        RED_BUCKET,
        GREEN_BUCKET,
        BLUE_BUCKET,
        YELLOW_BUCKET,
        GARBAGE_BUCKET
    };

    Segment *segment = get_segment(queue, LAST_INDEX);
    uint8_t position;

    if (segment->is_occupied && segment->object_type == BALL)
    {
        position = segment->color;
    }
    else
    {
        position = UNKNOWN;
    }

    if (position != last_position)
    {
        advanced_motor_turn_to_degree(separator, bucket_position[position]);
        last_position = position;
    }
}






















/*

void task_send_take_picture(Segment_Queue *queue)
{
    Segment *segment = get_segment(queue, KINECT_SEGMENT_INDEX);
    if (segment->is_occupied)
    {
        Message message;
        message.type = MESSAGE_TYPE_COMMAND;
        message.command.type = MESSAGE_COMMAND_TAKE_PICURE;

        io_send_message(&message);
    }
}

void task_request_object_info(Segment_Queue *segment_queue)
{
    Segment *segment = get_segment(segment_queue, KINECT_SEGMENT_INDEX);
    if (segment->is_occupied)
    {
        Message message;
        io_await_message(&message);

        if (message.type == MESSAGE_TYPE_OBJECT)
        {
            Segment *segment =
                get_segment(segment_queue, KINECT_SEGMENT_INDEX);

            segment->object_type = message.object.type;
            return;
        }
    }
    else
    {
        return;
    }

    ASSERT(false);
}
*/