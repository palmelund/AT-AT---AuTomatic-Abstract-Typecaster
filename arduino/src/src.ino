#include "src.h"

/* PLAN
 *
 * Startup command interface
 *  * Calibrate colors
 *  * Get color calibrations and measured distance
 *  * Start sorting machine
 *
 * Feed ball
 *  * feed_ball()
 * Ping color and Sound
 *  * Multiple code things
 * Send command
 *  * command_take_picture()
 * Request object
 *  * request_object_info()
 * Move separator
 *  * Multiple code things
 */

SFE_ISL29125 RGB_sensor;
Motor motor_conveyor, motor_feeder;
Advanced_Motor adv_motor_separator;
Ultra_Sound_Sensor distance_sensor;

uint16_t distance_to_wall;

// 0: Green
// 1: Yellow
// 2: Red
// 3: Blue
Delta_RGB colors[COLOR_COUNT];
Segment_Queue segment_queue;

// -------------------------------
// Arduino main functions
// -------------------------------
void setup()
{
    // Open serial connection
    Serial.begin(9600);
    while (!Serial)
        ;

#if DEBUGGING
    DEBUG_PRINTLN("Ready!");

    while (Serial.available() <= 0) ;
#endif

    // Initialize color sensor

    DEBUG_PRINTLN("Initializing all components...");
    DEBUG_PRINTLN("- Ultra sound sensor...");
    distance_sensor_init(&distance_sensor, RANGE_TRIG, RANGE_ECHO);

    DEBUG_PRINTLN("--- Calibrating...");
    distance_to_wall = calibrate_ultra_sound_sensor();
    DEBUG_PRINT("---");
    DEBUG_PRINTLN_VAR(distance_to_wall);

    DEBUG_PRINTLN("- Motors...");
    motor_init(&motor_conveyor, 0.36, MOTOR_CONVEYOR_PIN, MOTOR_CONVEYOR_INT_PIN,
               motor_conveyor_interrupt);
    motor_init(&motor_feeder, 1.0, MOTOR_FEEDER_PIN, MOTOR_FEEDER_INT_PIN,
               motor_feeder_interrupt);

    advanced_motor_init(&adv_motor_separator, 1.0, MOTOR_SEPARATOR_PIN1,
                        MOTOR_SEPARATOR_PIN2, MOTOR_SEPARATOR_INT_PIN1, MOTOR_SEPARATOR_DATA_PIN,
                        adv_motor_separator_interrupt1);

    DEBUG_PRINTLN("- Color sensor...");
    RGB_sensor.init();
    while (RGB_sensor.readRed() == 0 ||
           RGB_sensor.readGreen() == 0 ||
           RGB_sensor.readBlue() == 0)
        ;

    DEBUG_PRINTLN("Starting the sorting machine...");
    motor_turn(&motor_conveyor);

/*
    Delta_RGB red;
    calibrate_color(&red);

    while (true)
    {
        RGB color;
        read_color(&color);
        uint16_t delta = euclidean_distance_3d(&color, &red.rgb);

        DEBUG_PRINT_RGB(color);
        DEBUG_PRINT_RGB(red.rgb);
        DEBUG_PRINTLN_VAR(red.delta);
        DEBUG_PRINTLN_VAR(delta);

        if (delta < red.delta)
            DEBUG_PRINTLN("Yay");
        else
            DEBUG_PRINTLN("Nay");

        delay(300);
    }
    */
}

void test_color_com()
{
    while (true)
    {
        read_color(&colors[RED_BALL].rgb);
        //calibrate_color(&colors[RED]);
        //Serial.print("\n\nCALIBRATE: ");
        Serial.print(colors[RED_BALL].rgb.red);
        Serial.print(" ");
        Serial.print(colors[RED_BALL].rgb.green);
        Serial.print(" ");
        Serial.print(colors[RED_BALL].rgb.blue);
        Serial.println("");
    }
}

void startup_helper()
{
    pinMode(42, OUTPUT);
    bool start = false;
    while (!start)
    {
        In_Message message;
        io_await_message(&message);

        switch (message.type)
        {
        case IN_MESSAGE_COMMAND:
            switch (message.command.type)
            {
            case IN_COMMAND_CALIBRATE_RED:
                calibrate_color(&colors[RED_BALL]);

                //read_color(&colors[RED].rgb);

                Out_Message out;
                out.type = OUT_MESSAGE_COLOR;
                out.color.type = RED_BALL;

                out.color.red_value = colors[RED_BALL].rgb.red;
                out.color.green_value = colors[RED_BALL].rgb.green;
                out.color.blue_value = colors[RED_BALL].rgb.blue;
                io_send_message(&out);

                continue;
            case IN_COMMAND_CALIBRATE_GREEN:
                calibrate_color(&colors[GREEN_BALL]);
                continue;
            case IN_COMMAND_CALIBRATE_BLUE:
                calibrate_color(&colors[BLUE_BALL]);
                continue;
            case IN_COMMAND_CALIBRATE_DISTANCE:
                calibrate_color(&colors[YELLOW_BALL]);
                continue;
            case IN_COMMAND_START:
                start = true;
                continue;
            case IN_COMMAND_STOP:
            default:
                abort();
                continue;
            }
        case IN_MESSAGE_OBJECT:
        case IN_MESSAGE_COLOR:
        case IN_MESSAGE_DISTANCE:
        default:
            continue;
        }
    }
}

void loop()
{
    static bool run = true;

    if (Serial.available() > 0)
    {
        uint8_t c = Serial.read();
        if (c == 'b') // BEGIN
        {
            run = true;
            motor_turn(&motor_conveyor);
        }

        else if (c == 'e')  // END
        {
            run = false;
            motor_stop(&motor_conveyor);
        }
    }

    if (run == false)
        return;

    static int32_t conveyor_target = SEGMENT_DEGREE_LENGTH;

    

    while (motor_get_degrees(&motor_conveyor) < conveyor_target)
        ;
    conveyor_target += SEGMENT_DEGREE_LENGTH;
}

// -------------------------------
// Interrupt functions
// -------------------------------
void motor_conveyor_interrupt()
{
    motor_update_degrees(&motor_conveyor);
}

void motor_feeder_interrupt()
{
    motor_update_degrees(&motor_feeder);
}

void adv_motor_separator_interrupt1()
{
    advanced_motor_update_degrees(&adv_motor_separator);
}

// -------------------------------
// Ultra sound sensor functions
// -------------------------------
// TODO: filter out short distances
int32_t calibrate_ultra_sound_sensor()
{
    int32_t min = distance_sensor_measure_distance(&distance_sensor);
    int32_t current;
    for (uint8_t i = 0; i < CALIBRACTION_ITERATIONS - 1; i++)
    {
        // We delay a little, because it seems that calling measure_distance
        // to rapidly, affects the results.
        delay(5); 

        //DEBUG_PRINTLN_VAR(min);
        current = distance_sensor_measure_distance(&distance_sensor);

        if (current < min)
            min = current;
    }

    return min;
}

// -------------------------------
// Queue functions
// -------------------------------

Segment* queue_next(Segment_Queue *segment_queue)
{
    segment_queue->index++;
    if (segment_queue->index >= QUEUE_SIZE)
        segment_queue->index = 0;

    return &segment_queue->data[segment_queue->index];
}

Segment* get_segment(Segment_Queue *segment_queue, uint8_t offset)
{
    return &segment_queue->data[(segment_queue->index + offset) % QUEUE_SIZE];
}

// -------------------------------
// Functions for processing colors
// -------------------------------
char *get_color_name(uint8_t color)
{
    switch (color)
    {
    case GREEN_BALL:
        return (char *)"Green";
    case YELLOW_BALL:
        return (char *)"Yellow";
    case RED_BALL:
        return (char *)"Red";
    case BLUE_BALL:
        return (char *)"Blue";
    case NOT_BALL:
        return (char *)"Not Ball";
    default:
        ASSERT(false);
    }
}

void read_color(RGB *result)
{
    result->red = RGB_sensor.readRed();
    result->green = RGB_sensor.readGreen();
    result->blue = RGB_sensor.readBlue();
}

float euclidean_distance_3d(RGB *rgb1, RGB *rgb2)
{
    int16_t res1 = (int16_t)rgb1->red - (int16_t)rgb2->red;
    int16_t res2 = (int16_t)rgb1->green - (int16_t)rgb2->green;
    int16_t res3 = (int16_t)rgb1->blue - (int16_t)rgb2->blue;

    return (float)sqrt(pow(res1, 2) + pow(res2, 2) + pow(res3, 2));
}

void calibrate_color(Delta_RGB *result)
{
    // Read samples to calibrate from
    RGB samples[CALIBRACTION_ITERATIONS];
    for (uint8_t i = 0; i < CALIBRACTION_ITERATIONS; i++)
    {
        read_color(&samples[i]);
        //DEBUG_PRINT_RGB(samples[i]);
    }

    // https://en.wikipedia.org/wiki/Bounding_sphere
    // Using Ritter's bounding sphere method.
    // We start by picking a point in 3d space
    RGB *point1 = &samples[0];
    RGB *point2;

    float greatest_distance = 0;

    // Find the point furthest away from point1
    for (uint8_t i = 1; i < CALIBRACTION_ITERATIONS; i++)
    {
        float distance = euclidean_distance_3d(point1, &samples[i]);
        if (distance > greatest_distance)
        {
            greatest_distance = distance;
            point2 = &samples[i];
        }
    }

    greatest_distance = 0;

    // Find the point furthest away from point2
    for (uint8_t i = 0; i < CALIBRACTION_ITERATIONS; i++)
    {
        float distance = euclidean_distance_3d(point2, &samples[i]);
        if (distance > greatest_distance)
        {
            greatest_distance = distance;
            point1 = &samples[i];
        }
    }

    // We now construct a sphere which cotains the two points.
    // This is done by first, finding the point in between the two points
    result->rgb.red = (point1->red + point2->red) / 2;
    result->rgb.green = (point1->green + point2->green) / 2;
    result->rgb.blue = (point1->blue + point2->blue) / 2;

    // Then finding the radius of the sphere, by taking the distance
    // from the center to one of the two points.
    // NOTE: We calculate both, since rounding errors gives us a pointer
    //       not exactly in the center
    float distance_to_point1 = euclidean_distance_3d(&result->rgb, point1);
    float distance_to_point2 = euclidean_distance_3d(&result->rgb, point2);

    // Choose the greatest radius, ensures that both points are in the sphere
    if (distance_to_point1 > distance_to_point2)
        result->delta = (uint16_t)ceil(distance_to_point1);
    else
        result->delta = (uint16_t)ceil(distance_to_point2);

    ASSERT(result->delta >= distance_to_point1);
    ASSERT(result->delta >= distance_to_point2);

    // Now we need to ensure that the sphere we just found actually
    // contains all points
    for (;;)
    {
        RGB *outside_point = NULL;
        float distance_to_outside_point;

        // Checking if all points are contained
        for (uint8_t i = 0; i < CALIBRACTION_ITERATIONS; i++)
        {
            distance_to_outside_point = euclidean_distance_3d(&result->rgb, &samples[i]);
            if (distance_to_outside_point > result->delta)
            {
                outside_point = &samples[i];
                break;
            }
        }

        // If no points was outside the sphere, we know the sphere contains
        // all points, and the algorithm is done
        if (outside_point == NULL)
            break;

        // When a point is found outside the sphere, we need a new sphere
        // that contains all points in the previous sphere + the outside point.

        // We construct the sphere by extending the line from the center
        // of the sphere to the outside point by the radius of the sphere.
        // NOTE: http://math.stackexchange.com/questions/352828/increase-length-of-line
        float delta_red = ((float)result->rgb.red - outside_point->red) / distance_to_outside_point;
        float delta_green = ((float)result->rgb.green - outside_point->green) / distance_to_outside_point;
        float delta_blue = ((float)result->rgb.blue - outside_point->blue) / distance_to_outside_point;

        // The moved_point, will be the new point that is created from extending
        // that line.
        RGB moved_result;
        moved_result.red = (uint16_t)ceil(result->rgb.red + delta_red);
        moved_result.green = (uint16_t)ceil(result->rgb.green + delta_green);
        moved_result.blue = (uint16_t)ceil(result->rgb.blue + delta_blue);

        // We now construct the new sphere which cotains outside_point and
        // moved_result. This is the same as how we created our original sphere.
        result->rgb.red = (moved_result.red + outside_point->red) / 2;
        result->rgb.green = (moved_result.green + outside_point->green) / 2;
        result->rgb.blue = (moved_result.blue + outside_point->blue) / 2;

        uint16_t prev_delta = result->delta;
        float distance_to_moved_result = euclidean_distance_3d(&result->rgb, &moved_result);
        distance_to_outside_point = euclidean_distance_3d(&result->rgb, &moved_result);

        if (distance_to_moved_result > distance_to_outside_point)
            result->delta = (uint16_t)ceil(distance_to_moved_result);
        else
            result->delta = (uint16_t)ceil(distance_to_outside_point);

        ASSERT(result->delta >= distance_to_moved_result);
        ASSERT(result->delta >= distance_to_outside_point);
        ASSERT(result->delta >= prev_delta);

        // Now we repeat!
    }

    // Just to add some extra in case of noise
    result->delta += 100;
}

uint8_t determin_color(RGB *color)
{
    uint8_t closest_color = NOT_BALL;
    uint16_t closest_distance = USHRT_MAX;
    for (uint8_t i = 0; i < COLOR_COUNT; i++)
    {
        uint16_t distance = euclidean_distance_3d(color, &colors[i].rgb);
        if (colors[i].delta > distance && closest_distance > distance)
        {
            closest_distance = distance;
            closest_color = i;
        }
    }

    return closest_color;
}


// -------------------------------
// Task that are executed by the cyclic executive
// -------------------------------

void task_check_first_segment()
{
    int32_t test_dist = distance_sensor_measure_distance(&distance_sensor);

    DEBUG_PRINT_VAR(test_dist);
    DEBUG_PRINT(" ");
    DEBUG_PRINTLN_VAR(distance_to_wall);


    Segment* first_segment = queue_next(&segment_queue);

    // Tests if a ball is in front of sensor
    if (test_dist < distance_to_wall)
    {
        DEBUG_PRINTLN("Segment was occupied");
        first_segment->is_occupied = true;
    }
    else
    {
        DEBUG_PRINTLN("Segment was empty");
        first_segment->is_occupied = false;
    }
}
void task_send_take_picture()
{
    Out_Message message;
    message.type = OUT_MESSAGE_COMMAND;
    message.command.type = OUT_COMMAND_TAKE_PICURE;

    io_send_message(&message);
}

void task_determin_color()
{
    Segment* segment = get_segment(&segment_queue, COLOR_SENSOR_SEGMENT_INDEX);

    if (segment->is_occupied && segment->object_type == BALL)
    {
        RGB color;
        read_color(&color);
        uint8_t determined_color = determin_color(&color);
        segment->color = determined_color;
    }
}

void task_request_object_info()
{
    In_Message message;
    io_await_message(&message);

    if (message.type == IN_MESSAGE_OBJECT)
    {
        return message.object.type;
    }

    ASSERT(false);
}

void task_feed_ball()
{
    static uint8_t feed_counter = FEEDER_ITERATION;
    static int16_t deg = FEEDER_DEGREES;

    // We only feed a ball every x iterations
    if (feed_counter == FEEDER_ITERATION)
    {
        motor_turn_to_degree(&motor_feeder, deg);

        deg += FEEDER_DEGREES;
        if (deg == 360)
            deg = 0;

        feed_counter = 0;
    }

    feed_counter++;
}

void task_rotate_seperator()
{
    static int16_t bucket_pos[BUCKET_COUNT] = { 
        GREEN_BUCKET, 
        YELLOW_BUCKET, 
        RED_BUCKET, 
        BLUE_BUCKET, 
        GARBAGE_BUCKET
    };
    static Segment last_occupied_segment;

    Segment* segment = get_segment(&segment_queue, LAST_INDEX);

    // We only wonna move the buckets, if the segment is occupied, and
    // is different from the last occupied segment
    if (segment->is_occupied && 
        last_occupied_segment->object_type != segment->object_type)
    {
        DEBUG_PRINT("ejecting: ");
        DEBUG_PRINTLN(get_color_name(current_ball));
        advanced_motor_turn_to_degree(&adv_motor_separator,
                                    bucket_pos[segment->object_type]);

        last_occupied_segment = *segment;
    }
}