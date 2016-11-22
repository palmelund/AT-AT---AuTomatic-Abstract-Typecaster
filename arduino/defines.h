// Motor 2: Control conveyor belt
// Motor 3: Control feeder
// Motor 4: Control separator

#ifndef _DEFINES_H_
#define _DEFINES_H_
#include <stddef.h>

#define MOTOR_CONVEYOR_PIN 27            // Power conveyor motor
#define MOTOR_FEEDER_PIN 29             // Power feeder motor
#define MOTOR_SEPARATOR_PIN1 25         // Control separator motor direction
#define MOTOR_SEPARATOR_PIN2 23         // Control separator motor direction

#define MOTOR_CONVEYOR_INT_PIN 2       // Interrupt conveyor motor
#define MOTOR_FEEDER_INT_PIN 18          // Interrupt feeder motor
#define MOTOR_SEPARATOR_INT_PIN1 3      // Interrupt separator motor
#define MOTOR_SEPARATOR_DATA_PIN 51     // Used to read direction, not an actual 
                                        // interrupt

#define BUTTON_INT_PIN 19               // Emergency stop int port

#define RANGE_ECHO 53                   // Read distance
#define RANGE_TRIG 31                   // Start the sensor

#define QUEUE_SIZE 6                    // Size of the conveyor queue

#define CALIBRACTION_ITERATIONS 100

#define LED_RED_PIN
#define LED_YELLOW_PIN
#define LED_GREEN_PIN

#define DEBUGGING 0

#if DEBUGGING
#define DEBUG_PRINT(x) Serial.print(x)
#define DEBUG_PRINTLN(x) Serial.println(x)
#else
#define DEBUG_PRINT(x)
#define DEBUG_PRINTLN(x)
#endif

#if DEBUGGING
#define DEBUG_PRINT_VAR(var) \
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

#endif // _DEFINES_H_