#ifndef _COMPONENT_TESTS_H_
#define _COMPONENT_TESTS_H_

#include "motor_api.h"
#include "Arduino.h"

//#define COMPONENT_TEST_MOTOR_COAST
//#define COMPONENT_TEST_MOTOR_TIME
//#define COMPONENT_TEST_INTERRUPT_COST
//#define COMPONENT_TEST_MOTOR_DEGREE
//#define API_TEST_MOTOR_TURN_DEG
#define COMPONENT_TEST_COLOR

#ifdef COMPONENT_TEST_MOTOR_COAST
void component_test_motor_coast(Motor* motor);
#endif

#ifdef COMPONENT_TEST_MOTOR_TIME
void component_test_motor_time(Motor* motor);
#endif

#ifdef COMPONENT_TEST_INTERRUPT_COST
void component_test_interupt_cost(Advanced_Motor* motor);
#endif

#ifdef COMPONENT_TEST_MOTOR_DEGREE
//TODO: TEST
#endif

#ifdef API_TEST_MOTOR_TURN_DEG
//TODO: TEST
#endif

#ifdef COMPONENT_TEST_COLOR
#include "SparkFunISL29125.h"
#include "colors.h"
void component_test_color();

#endif

#endif  // _COMPONENT_TESTS_H_