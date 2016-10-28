#ifndef MOTOR_API_H
#define MOTOR_API_H

#include "Arduino.h"
#include <stdint.h>

/*
 * The directions a motor can turn.
 * These are also used by an advanced motor to increment or derement the degrees
 */
enum Turning_Direction 
{
  FORWARD = -1,
  BACKWARD = 1
};

/*
 * The base data for all motor types
 */
struct Base_Motor 
{
  volatile int32_t degrees = 0;
  volatile int32_t buffer = 0;
  volatile bool reading = false;
  float degree_ratio = 1;
};

/*
 * The data for a motor
 */
struct Motor 
{
  Base_Motor base;
  uint8_t pin;
};

/*
 * The data for an advanced motor.
 * An advanced motor can turn in both directions
 */
struct Advanced_Motor 
{
  Base_Motor base;
  uint8_t pin1;
  uint8_t pin2;
  uint8_t interrupt_pin1;
  uint8_t interrupt_pin2;
  volatile Turning_Direction direction;
};

/*
 * Initializes a standard motor
 */
void motor_init(Motor *motor, float degree_ratio, uint8_t pin, 
  uint8_t interrupt_pin, void (*interrupt_handler)(void));

/*
 * Initializes an advanced motor
 */
void advanced_motor_init(Advanced_Motor *motor, float degree_ratio, 
  uint8_t pin1, uint8_t pin2, uint8_t interrupt_pin1, uint8_t interrupt_pin2, 
  void (*interrupt_handler1)(void));

/*
 * Turns an advanced motor to an angle
 */
void advanced_motor_turn_to_degree(Advanced_Motor* motor, uint16_t degree);

/*
 * Turns an advanced motor a number of degrees in a certin direction
 */
void advanced_motor_turn_degrees(Advanced_Motor* motor, uint16_t degrees, 
  Turning_Direction direction);

/*
 * Turns a motor to an angle
 */
void motor_turn_to_degree(Motor* motor, uint16_t degree);

/*
 * Turns a motor a number of degrees
 */
void motor_turn_degrees(Motor* motor, uint16_t degrees);

/*
 * Tries to stop an advanced motor with minimum coasting
 */
void advanced_motor_stop(Advanced_Motor* motor);

/*
 * Stops a motor
 */
void motor_stop(Motor* motor);

/*
 * Make an advanced motor turn in a certin direction by an analog value
 */
void advanced_motor_turn_analog(Advanced_Motor *motor, 
  Turning_Direction direction, uint8_t value);

/*
 * Make an advanced motor turn in a certin direction
 */
void advanced_motor_turn(Advanced_Motor *motor, Turning_Direction direction);

/*
 * Make a motor turn by an analog value
 */
void motor_turn_analog(Motor *motor, uint8_t value);

/*
 * Make a motor turn
 */
void motor_turn(Motor* motor);

/*
 * Safely retriving(locking) the degrees from a motor
 */
int32_t motor_get_degrees(Motor *motor);

/*
 * Safely retriving(locking) the degrees from an advanced motor
 */
int32_t advanced_motor_get_degrees(Advanced_Motor *motor);

/*
 * Updates the degree value on a motor. 
 * Should either be scheduled or attached to an interrupt
 */
void motor_update_degrees(Motor* motor);

/*
 * Updates the degree and direction value on an advanced motor. 
 * Should either be scheduled or attached to an interrupt
 */
void advanced_motor_update_degrees(Advanced_Motor* motor);

#endif