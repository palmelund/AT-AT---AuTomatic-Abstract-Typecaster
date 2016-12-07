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

/*
RED: 957, 1139, 966 - 106
GREEN: 707, 1183, 1030 - 156
BLUE: 716, 1110, 1228 - 153
YELLOW: 1246, 1799, 1173 - 309

RED: colors[i].delta: 204 - colors[i].rgb r: 941 g: 1147 b: 951
GREEN: colors[i].delta: 225 - colors[i].rgb r: 699 g: 1154 b: 978
BLUE: colors[i].delta: 234 - colors[i].rgb r: 698 g: 1122 b: 1134
YELLOW: colors[i].delta: 452 - colors[i].rgb r: 1204 g: 1699 b: 1099
BELT: colors[i].delta: 235 - colors[i].rgb r: 708 g: 1087 b: 940
*/

// 0: RED
// 1: GREEN
// 2: BLUE
// 3: YELLOW
Delta_RGB colors[COLOR_COUNT] = {
    {{941, 1147, 951}, 204},
    {{699, 1154, 978}, 225},
    {{698, 1122, 1134}, 234},
    {{1204, 1699, 1099}, 452},
    {{708, 1087, 940}, 235}};
Segment_Queue segment_queue;

int32_t conveyor_target;

// -------------------------------
// Arduino main functions
// -------------------------------
void setup()
{
    // Open serial connection
    Serial.begin(9600);
    while (!Serial)
        ;

    // Initialize color sensor
    RGB_sensor.init();

    DEBUG_PRINTLN("Initializing all components...");
    DEBUG_PRINTLN("- Ultra sound sensor...");
    distance_sensor_init(&distance_sensor, RANGE_TRIG, RANGE_ECHO);

    DEBUG_PRINTLN("- Motors...");
    motor_init(&motor_conveyor, 0.36, MOTOR_CONVEYOR_PIN, MOTOR_CONVEYOR_INT_PIN,
               motor_conveyor_interrupt);
    motor_init(&motor_feeder, 1.0, MOTOR_FEEDER_PIN, MOTOR_FEEDER_INT_PIN,
               motor_feeder_interrupt);

    advanced_motor_init(&adv_motor_separator, 1.0, MOTOR_SEPARATOR_PIN1,
                        MOTOR_SEPARATOR_PIN2, MOTOR_SEPARATOR_INT_PIN1, MOTOR_SEPARATOR_DATA_PIN,
                        adv_motor_separator_interrupt1);

    DEBUG_PRINTLN("- Color sensor...");
    while (RGB_sensor.readRed() == 0 ||
           RGB_sensor.readGreen() == 0 ||
           RGB_sensor.readBlue() == 0)
        ;
    motor_stop(&motor_conveyor);

#if DEBUGGING

    DEBUG_PRINTLN("Ready!");

    while (Serial.available() <= 0)
        ;
    Serial.read();
#else
    for (;;)
    {
        In_Message message;
        io_await_message(&message);
        if (message.type == IN_MESSAGE_COMMAND)
        {
            if (message.command.type == IN_COMMAND_START)
            {
                break;
            }
        }
    }
#endif

    /*
    while (true)
    {
        task_check_first_segment(&distance_sensor, distance_to_wall, &segment_queue);

        RGB color;
        read_color(color_sensor, &color);
        //uint8_t determined_color = determin_color(known_colors, &color);
        DEBUG_PRINT_RGB(color);
        //DEBUG_PRINT("C: ");
        //DEBUG_PRINTLN(get_color_name(determined_color));

        while (motor_get_degrees(&motor_conveyor) < conveyor_target)
            ;
        conveyor_target += SEGMENT_DEGREE_LENGTH;
    }
    */
    motor_turn(&motor_conveyor);
    advanced_motor_turn_to_degree(&adv_motor_separator, GARBAGE_BUCKET);

#if CALIBRATE_COLORS
    conveyor_target = SEGMENT_DEGREE_LENGTH * 2;

    DEBUG_PRINTLN("Calibrating colors...");
    for (uint8_t i = 0; i < COLOR_COUNT; ++i)
    {
        calibrate_color(&RGB_sensor, &colors[i]);

        DEBUG_PRINT_VAR(colors[i].delta);
        DEBUG_PRINT(" - ");
        DEBUG_PRINT_RGB(colors[i].rgb);

        while (motor_get_degrees(&motor_conveyor) < conveyor_target)
            ;
        conveyor_target += SEGMENT_DEGREE_LENGTH * 2;
    }

    DEBUG_PRINTLN("--- Calibrating...");
    distance_to_wall = task_calibrate_ultra_sound_sensor(&distance_sensor);
    DEBUG_PRINT("---");
    DEBUG_PRINTLN_VAR(distance_to_wall);

    DEBUG_PRINTLN("Done!");
#endif

    //wce_task_check_first_segment();
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

        else if (c == 'e') // END
        {
            run = false;
            motor_stop(&motor_conveyor);
        }
    }

    if (run == false)
        return;

    task_check_first_segment(&distance_sensor, distance_to_wall, &segment_queue);
    task_send_take_picture(&segment_queue);
    //print_queue();
    task_determin_color(&RGB_sensor, &segment_queue, colors);
    //task_request_object_info(&segment_queue);
    task_rotate_seperator(&adv_motor_separator, &segment_queue);
    task_feed_ball(&motor_feeder);

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
                calibrate_color(&RGB_sensor, &colors[RED]);

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
                calibrate_color(&RGB_sensor, &colors[GREEN]);
                continue;
            case IN_COMMAND_CALIBRATE_BLUE:
                calibrate_color(&RGB_sensor, &colors[BLUE]);
                continue;
            case IN_COMMAND_CALIBRATE_DISTANCE:
                calibrate_color(&RGB_sensor, &colors[YELLOW]);
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

void print_queue()
{
    for (int8_t i = QUEUE_SIZE - 1; i >= 0; --i)
    {
        Serial.print("|");
        Segment *segment = get_segment(&segment_queue, i);

        if (segment->is_occupied)
        {
            if (segment->object_type == BALL)
                Serial.print(get_color_name(segment->color));
            else
                Serial.print("O");
        }
        else
        {
            Serial.print(" ");
        }
    }
    Serial.println("|");
}