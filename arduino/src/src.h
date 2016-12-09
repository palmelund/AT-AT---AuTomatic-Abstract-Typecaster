#ifndef _SRC_H_
#define _SRC_H_

#include "defines.h"
#include "motor_api.h"
#include "distance_sensor_api.h"
#include "SparkFunISL29125.h"
#include "io_api.h"
#include "colors.h"
#include "segments.h"
#include "tasks.h"
#include <stdint.h>

#define CALIBRATE_COLORS 1

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

#endif // _SRC_H_