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

SFE_ISL29125 rgb_sensor;
Motor motor_conveyor, motor_feeder;
Advanced_Motor adv_motor_separator;

// 0: RED
// 1: GREEN
// 2: BLUE
// 3: YELLOW
// 4: BACKGROUND
Delta_RGB colors[COLOR_COUNT] = {
    {{941, 1147, 951}, 204},
    {{699, 1154, 978}, 225},
    {{698, 1122, 1134}, 234},
    {{1204, 1699, 1099}, 452}
    //{{708, 1087, 940}, 235}
};

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
    rgb_sensor.init();

    DEBUG_PRINTLN("Initializing all components...");

    DEBUG_PRINTLN("- Motors...");
    
    motor_init(&motor_conveyor, 0.20, MOTOR_CONVEYOR_PIN, 
               MOTOR_CONVEYOR_INT_PIN, MOTOR_CONVEYOR_DATA_PIN,
               motor_conveyor_interrupt);
    motor_init(&motor_feeder, 1.0, MOTOR_FEEDER_PIN, 
               MOTOR_FEEDER_INT_PIN, MOTOR_FEEDER_DATA_PIN,
               motor_feeder_interrupt);

    advanced_motor_init(&adv_motor_separator, 1.0, MOTOR_SEPARATOR_INT_PIN,
                        MOTOR_SEPARATOR_PIN2, MOTOR_SEPARATOR_INT_PIN, MOTOR_SEPARATOR_DATA_PIN,
                        adv_motor_separator_interrupt1);

    DEBUG_PRINTLN("- Color sensor...");
    while (rgb_sensor.readRed() == 0 ||
           rgb_sensor.readGreen() == 0 ||
           rgb_sensor.readBlue() == 0)
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
        Message message;
        io_await_message(&message);
        if (message.type == MESSAGE_TYPE_COMMAND &&
            message.command.type == MESSAGE_COMMAND_START)
        {
            break;
        }
    }
#endif

    motor_turn(&motor_conveyor);
    advanced_motor_turn_to_degree(&adv_motor_separator, GARBAGE_BUCKET);

#if CALIBRATE_COLORS
    task_calibrate_colors(&conveyor_target, &motor_conveyor, &rgb_sensor,
                          colors);
#endif
}

void loop()
{
    task_check_first_segment(&segment_queue);

    task_determin_color(&rgb_sensor, &segment_queue, colors);
    task_rotate_seperator(&adv_motor_separator, &segment_queue);
    task_feed_ball(&motor_feeder);

    if (motor_get_degrees(&motor_conveyor) >= conveyor_target)
    {
        // This will only happen if a deadline is missed
        ASSERT(false);
    }

    while (motor_get_degrees(&motor_conveyor) < conveyor_target)
    { }
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