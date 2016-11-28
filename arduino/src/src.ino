#include "src.h"

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

// -------------------------------
// Arduino main functions
// -------------------------------
void setup()
{
    Serial.begin(9600);
    while (!Serial)
        ;

    if (!RGB_sensor.init())
    {
        while (true)
        {
            digitalWrite(42, HIGH);
            delay(100);
            digitalWrite(42, LOW);
            delay(100);
        }
    }

    while (RGB_sensor.readRed() == 0 || RGB_sensor.readGreen() == 0 || RGB_sensor.readBlue() == 0)
        ;

  //exit(0);

  DEBUG_PRINTLN("Initializing all components...");
  DEBUG_PRINTLN("- Ultra sound sensor...");
  distance_sensor_init(&distance_sensor, RANGE_TRIG, RANGE_ECHO);

  DEBUG_PRINTLN("--- Calibrating...");
  distance_to_wall = calibrate_ultra_sound_sensor();
  DEBUG_PRINT("---"); DEBUG_PRINTLN_VAR(distance_to_wall);

  DEBUG_PRINTLN("- Motors...");
  motor_init(&motor_conveyor, 0.36, MOTOR_CONVEYOR_PIN, MOTOR_CONVEYOR_INT_PIN, 
    motor_conveyor_interrupt);
  motor_init(&motor_feeder, 1.0, MOTOR_FEEDER_PIN, MOTOR_FEEDER_INT_PIN, 
    motor_feeder_interrupt);

  advanced_motor_init(&adv_motor_separator, 1.0, MOTOR_SEPARATOR_PIN1, 
    MOTOR_SEPARATOR_PIN2,MOTOR_SEPARATOR_INT_PIN1, MOTOR_SEPARATOR_DATA_PIN, 
    adv_motor_separator_interrupt1);

    Serial.print("Distance: ");
    Serial.println(distance_to_wall);

    while(true)
    {
        int32_t distance = distance_sensor_measure_distance(&distance_sensor);

        if (distance + 10 < distance_to_wall || distance > distance_to_wall + 10)
        {
        Serial.println(distance_sensor_measure_distance(&distance_sensor));
        
        }
        delay(100);
    }

/*

  DEBUG_PRINTLN("Starting the sorting machine...");
  motor_turn_analog(&motor_conveyor, 255);
  */
    /*
                calibrate_color(&colors[RED]);

                Out_Message out;
                out.type = OUT_MESSAGE_COLOR;
                out.color.type = RED;

                out.color.red_value = colors[RED].rgb.red;
                out.color.green_value = colors[RED].rgb.green;
                out.color.blue_value = colors[RED].rgb.blue;
                io_send_message(&out);
*/
    //startup_helper();
    /*
    calibrate_color(&colors[RED]);
    
    Serial.print("Red: ");
    Serial.println(colors[RED].rgb.red);
    Serial.print("Green: ");
    Serial.println(colors[RED].rgb.green);
    Serial.print("Blue: ");
    Serial.println(colors[RED].rgb.blue);
    Serial.print("Delta: ");
    Serial.println(colors[RED].delta);
  */
    /*
    for (;;)
    {
        In_Message message_received;
        io_await_message(&message_received);

        Out_Message message;
        switch (message_received.type)
        {
        case IN_MESSAGE_COLOR:
            message.type = OUT_MESSAGE_COLOR;
            message.color.type = message_received.color.type;
            message.color.value = message_received.color.value;
            io_send_message(&message);
            break;

        case IN_MESSAGE_COMMAND:
            message.type = OUT_MESSAGE_COMMAND;
            message.command.type = message_received.command.type;
            io_send_message(&message);
            break;

        case IN_MESSAGE_DISTANCE:
            message.type = OUT_MESSAGE_DISTANCE;
            message.distance.value = message_received.distance.value;
            io_send_message(&message);
            break;

        case IN_MESSAGE_OBJECT:
            message.type = OUT_MESSAGE_REQUEST;
            message.request.type = OUT_REQUEST_OBJECT_INFO;
            io_send_message(&message);

        default:
            break;
        }
    }
    */

    test_color_com();
}

void test_color_com()
{
    while (true)
    {
        calibrate_color(&colors[RED]);
        Serial.print("\n\nCALIBRATE: ");
        Serial.print(colors[RED].rgb.red);
        Serial.print(" ");
        Serial.print(colors[RED].rgb.green);
        Serial.print(" ");
        Serial.print(colors[RED].rgb.blue);
        Serial.println("\n\n");
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
                calibrate_color(&colors[RED]);

                //read_color(&colors[RED].rgb);

                Out_Message out;
                out.type = OUT_MESSAGE_COLOR;
                out.color.type = RED;

                out.color.red_value = colors[RED].rgb.red;
                out.color.green_value = colors[RED].rgb.green;
                out.color.blue_value = colors[RED].rgb.blue;
                io_send_message(&out);

                continue;
            case IN_COMMAND_CALIBRATE_GREEN:
                calibrate_color(&colors[GREEN]);
                continue;
            case IN_COMMAND_CALIBRATE_BLUE:
                calibrate_color(&colors[BLUE]);
                continue;
            case IN_COMMAND_CALIBRATE_DISTANCE:
                calibrate_color(&colors[YELLOW]);
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
    return;
    static int16_t bucket_pos[5] = {0, 50, 100, 260, 310};
    static Segment_Queue segment_queue;
    static Ball_Color last_ball = EMPTY;
    static int32_t conveyor_target = 90;

    Ball_Color read_color = EMPTY;
    int32_t test_dist = distance_sensor_measure_distance(&distance_sensor);

    DEBUG_PRINT_VAR(test_dist);
    DEBUG_PRINT(" ");
    DEBUG_PRINTLN_VAR(distance_to_wall);

    // Tests if a ball is in front of sensor
    if (test_dist < distance_to_wall)
    {
        //read_color = read_color_sensor();
        DEBUG_PRINT("ball found: ");
        DEBUG_PRINTLN(get_color_name(read_color));
    }

    enqueue_segment(&segment_queue, read_color);
    feed_ball();

    Ball_Color current_ball = peek_segment(&segment_queue);

    // We only wonna move the buckets, if a ball was found, and that ball is
    // different from the last ball
    if (current_ball != EMPTY && current_ball != last_ball)
    {
        DEBUG_PRINT("ejecting: ");
        DEBUG_PRINTLN(get_color_name(current_ball));
        advanced_motor_turn_to_degree(&adv_motor_separator,
                                      bucket_pos[current_ball]);

        last_ball = current_ball;
    }

    while (motor_get_degrees(&motor_conveyor) < conveyor_target)
        ;
    conveyor_target += 90;
}

void feed_ball()
{
    const uint8_t feed_iteration = 8;
    static uint8_t feed_counter = feed_iteration;
    static int16_t deg = 90;

    // We only feed a ball every x iterations
    if (feed_counter == feed_iteration)
    {
        motor_turn_to_degree(&motor_feeder, deg);

        deg += 90;
        if (deg == 360)
            deg = 0;

        feed_counter = 0;
    }

    feed_counter++;
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
int32_t calibrate_ultra_sound_sensor()
{
    int32_t min = distance_sensor_measure_distance(&distance_sensor);
    int32_t current;
    for (uint8_t i = 0; i < CALIBRACTION_ITERATIONS - 1; i++)
    {
        DEBUG_PRINTLN_VAR(min);
        current = distance_sensor_measure_distance(&distance_sensor);

        if (current < min)
            min = current;
    }

    return min;
}

// -------------------------------
// Queue functions
// -------------------------------
void enqueue_segment(Segment_Queue *segment_queue, Ball_Color segment)
{
    segment_queue->data[segment_queue->index] = segment;

    segment_queue->index++;
    if (segment_queue->index >= QUEUE_SIZE)
        segment_queue->index = 0;
}

Ball_Color peek_segment(Segment_Queue *segment_queue)
{
    return segment_queue->data[segment_queue->index];
}

// -------------------------------
// Functions for processing colors
// -------------------------------
char *get_color_name(Ball_Color color)
{
    switch (color)
    {
    case GREEN:
        return (char *)"Green";
    case YELLOW:
        return (char *)"Yellow";
    case RED:
        return (char *)"Red";
    case BLUE:
        return (char *)"Blue";
    case EMPTY:
        return (char *)"Empty";
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
}

Ball_Color determin_color(RGB *color)
{
    uint8_t closest_color = EMPTY;
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

void command_take_picture()
{
    Out_Message message;
    message.type = OUT_MESSAGE_COMMAND;
    message.command.type = OUT_COMMAND_TAKE_PICURE;
    
    io_send_message(&message);
}

uint8_t request_object_info()
{
    In_Message message;
    io_await_message(&message);

    if (message.type == IN_MESSAGE_OBJECT)
    {
        return message.object.type;
    }
}