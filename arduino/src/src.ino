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

    while(RGB_sensor.readRed() == 0 || RGB_sensor.readGreen() == 0 || RGB_sensor.readBlue() == 0);

    /*
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
    startup_helper();
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
                //calibrate_color(&colors[RED]);

                read_color(&colors[RED].rgb);

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

uint16_t euclidean_distance_3d(RGB *rgb1, RGB *rgb2)
{
    int16_t res1 = (int16_t)rgb1->red - (int16_t)rgb2->red;
    int16_t res2 = (int16_t)rgb1->green - (int16_t)rgb2->green;
    int16_t res3 = (int16_t)rgb1->blue - (int16_t)rgb2->blue;

    return (uint16_t)sqrt(res1 * res1 + res2 * res2 + res3 * res3);
}

void calibrate_color(Delta_RGB *result)
{
    
    RGB samples[CALIBRACTION_ITERATIONS];
    for (uint8_t i = 0; i < CALIBRACTION_ITERATIONS; i++)
    {
        read_color(&samples[i]);
        /*
        Serial.print(samples[i].red);
        Serial.print(" ");
        Serial.print(samples[i].green);
        Serial.print(" ");
        Serial.println(samples[i].blue);
        */
    }

    
    RGB *point1 = &samples[0];
    RGB *point2;

    uint16_t greatest_distance = 0;

    // Find the point furthest away from point1
    for (uint8_t i = 1; i < CALIBRACTION_ITERATIONS; i++)
    {
        uint16_t distance = euclidean_distance_3d(point1, &samples[i]);
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
        uint16_t distance = euclidean_distance_3d(point2, &samples[i]);
        if (distance > greatest_distance)
        {
            greatest_distance = distance;
            point1 = &samples[i];
        }
    }

    result->rgb.red = (point1->red + point2->red) / 2;
    result->rgb.green = (point1->green + point2->green) / 2;
    result->rgb.blue = (point1->blue + point2->blue) / 2;
    result->delta = greatest_distance / 2;

    // Find the circle that can contain all samples
    for (;;)
    {
        RGB *outside_point = NULL;

        for (uint8_t i = 0; i < CALIBRACTION_ITERATIONS; i++)
        {
            if (euclidean_distance_3d(&result->rgb, &samples[i]) > result->delta)
            {
                outside_point = &samples[i];
                break;
            }
        }

        if (outside_point == NULL)
            break;

        uint16_t distance = euclidean_distance_3d(&result->rgb, outside_point);

        RGB delta;
        delta.red = (result->rgb.red - outside_point->red) / distance;
        delta.green = (result->rgb.green - outside_point->green) / distance;
        delta.blue = (result->rgb.blue - outside_point->blue) / distance;

        result->rgb.red = result->rgb.red + delta.red;
        result->rgb.green = result->rgb.green + delta.green;
        result->rgb.blue = result->rgb.blue + delta.blue;
        result->delta = (distance + result->delta) / 2;
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