// Motor 2: Control conveyor belt
// Motor 3: Control feeder
// Motor 4: Control separator

#ifndef _DEFINES_H_
#define _DEFINES_H_
#include <stddef.h>
#include "Arduino.h"

#define DEBUGGING 0 // TODO: Set to 0 when running release
#define RELEASEDEBUG 0

#define CALIBRACTION_ITERATIONS 200


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
#define ASSERT(logic) \
if (!(logic)) {                                 \
pinMode(42, OUTPUT); \
while (true) \
{ \
    digitalWrite(42, HIGH); \
    delay(500); \
    digitalWrite(42, LOW); \
    delay(500); \
}   \
}
#endif

#if RELEASEDEBUG
#define BLINK                       \
pinMode(42, OUTPUT); \
digitalWrite(42, HIGH); \
delay(500); \
digitalWrite(42, LOW)
#else
#define BLINK
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