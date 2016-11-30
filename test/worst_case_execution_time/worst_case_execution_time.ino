#include "worst_case_execution_time.h"

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
    distance_to_wall = task_calibrate_ultra_sound_sensor(&distance_sensor);
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

    #ifdef WCE_TEST_CHECK_FIRST_SEGMENT
    wce_task_check_first_segment(&motor_conveyor, &motor_feeder,
        &adv_motor_separator, &distance_sensor, distance_to_wall, 
        &segment_queue);
    #endif

    #ifdef WCE_TEST_SEND_TAKE_PICTURE
    wce_task_send_take_picture(&motor_conveyor, &motor_feeder,
        &adv_motor_separator);
    #endif

    #ifdef WCE_TEST_DETERMIN_COLOR
    wce_determin_color(&motor_conveyor, &motor_feeder,
        &adv_motor_separator, &RGB_sensor, &segment_queue, 
        colors);
    #endif

    #ifdef WCE_TEST_FEED_BALL
    wce_task_feed_ball(&motor_conveyor, &motor_feeder,
        &adv_motor_separator);
    #endif

    #ifdef WCE_TEST_ROTATE_SEPERATOR
    wce_task_rotate_seperator(&motor_conveyor, &motor_feeder,
        &adv_motor_separator, &segment_queue);
    #endif
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

void loop()
{

}