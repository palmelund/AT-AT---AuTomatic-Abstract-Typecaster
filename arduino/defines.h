// Motor 2: Control conveyor belt
// Motor 3: Control feeder
// Motor 4: Control separator

#ifndef _DEFINES_H_
#define _DEFINES_H_
#include <stddef.h>
#include "Arduino.h"

#define DEBUGGING 1

#define MOTOR_CONVEYOR_PIN 27           // Power conveyor motor
#define MOTOR_FEEDER_PIN 29             // Power feeder motor
#define MOTOR_SEPARATOR_PIN1 25         // Control separator motor direction
#define MOTOR_SEPARATOR_PIN2 23         // Control separator motor direction

#define MOTOR_CONVEYOR_INT_PIN 2        // Interrupt conveyor motor
#define MOTOR_FEEDER_INT_PIN 18         // Interrupt feeder motor
#define MOTOR_SEPARATOR_INT_PIN1 3      // Interrupt separator motor
#define MOTOR_SEPARATOR_DATA_PIN 51     // Used to read direction, not an actual 
                                        // interrupt

#define BUTTON_INT_PIN 19               // Emergency stop int port

#define RANGE_ECHO 53                   // Read distance
#define RANGE_TRIG 31                   // Start the sensor

#define LED_YELLOW_PIN
#define LED_GREEN_PIN
#define LED_RED_PIN

#define CALIBRACTION_ITERATIONS 100
#define FEEDER_ITERATION 8              // The iterations between each feed
#define FEEDER_DEGREES 90               // The degrees the feeder needs to turn
                                        // to feed a ball.
#define SEGMENT_DEGREE_LENGTH 72        // The number of degrees it takes for 
                                        // the conveyor belt motor to move one
                                        // segment.
#define QUEUE_SIZE 7                    // Size of the conveyor queue
#define SOUND_SENSOR_SEGMENT_INDEX 0    // The index, where the sounds sensor 
                                        // is positioned.
#define KINECT_SEGMENT_INDEX            // TODO: The index, where the kinect is
                                        // positioned.
#define COLOR_SENSOR_SEGMENT_INDEX 5    // The index, where the color sensor
                                        // is positioned.
#define LAST_INDEX QUEUE_SIZE - 1       // The index of the last segment.


#if DEBUGGING
#define DEBUG_PRINT(x) Serial.print(x)
#define DEBUG_PRINTLN(x) Serial.println(x)
#else
#define DEBUG_PRINT(x)
#define DEBUG_PRINTLN(x)
#endif

#if DEBUGGING
#define DEBUG_PRINT_VAR(var)    \
DEBUG_PRINT(#var);  \
DEBUG_PRINT(": ");  \
DEBUG_PRINT(var)
#else
#define DEBUG_PRINT_VAR(var)
#endif

#if DEBUGGING
#define DEBUG_PRINTLN_VAR(var) DEBUG_PRINT_VAR(var); DEBUG_PRINTLN("")
#else
#define DEBUG_PRINTLN_VAR(var)
#endif

#if DEBUGGING
#define ASSERT(logic)                           \
if (!(logic)) {                                 \
    Serial.println("--- Asserted ---");         \
    Serial.println(__func__);                   \
    Serial.println(__FILE__);                   \
    Serial.println(__LINE__);                   \
    Serial.println(#logic);                     \
    Serial.flush();                             \
    delay(1000);                                \
    abort();                                    \
}
#else
#define ASSERT(logic)
#endif

#endif // _DEFINES_H_

#define DEBUG_PRINT_RGB(var)    \
    DEBUG_PRINT(#var);          \
    DEBUG_PRINT(" r: ");        \
    DEBUG_PRINT(var.red);       \
    DEBUG_PRINT(" g: ");        \
    DEBUG_PRINT(var.green);     \
    DEBUG_PRINT(" b: ");        \
    DEBUG_PRINTLN(var.blue);