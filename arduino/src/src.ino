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
    RGB_sensor.init();

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
    while (RGB_sensor.readRed() == 0 ||
           RGB_sensor.readGreen() == 0 ||
           RGB_sensor.readBlue() == 0)
        ;

    DEBUG_PRINTLN("Starting the sorting machine...");
    //motor_turn(&motor_conveyor);

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

/*
 * Worst case execution time testing
 */