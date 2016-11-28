#include "motor_api.h"

/************************
 * Non public functions *
 ************************/
int32_t base_motor_get_degrees(Base_Motor *motor)
{
    int32_t res;

    // Mark reading as true, so no race conditions occur
    motor->reading = true;
    res = motor->degrees;
    motor->reading = false;

    return res * motor->degree_ratio;
}

/********************
 * Public functions *
 ********************/
void motor_init(Motor *motor, float degree_ratio, uint8_t pin,
                uint8_t interrupt_pin, void (*interrupt_handler)(void))
{
    motor->pin = pin;
    motor->base.degree_ratio = degree_ratio;
    motor->base.buffer = 0;
    motor->base.degrees = 0;
    motor->base.reading = false;

    pinMode(interrupt_pin, INPUT_PULLUP);
    pinMode(pin, OUTPUT);
    attachInterrupt(digitalPinToInterrupt(interrupt_pin), interrupt_handler,
                    CHANGE);
}

void advanced_motor_init(Advanced_Motor *motor, float degree_ratio,
                         uint8_t pin1, uint8_t pin2, uint8_t interrupt_pin1, uint8_t interrupt_pin2,
                         void (*interrupt_handler1)(void))
{
    motor->pin1 = pin1;
    motor->pin2 = pin2;
    motor->interrupt_pin1 = interrupt_pin1;
    motor->interrupt_pin2 = interrupt_pin2;
    motor->base.degree_ratio = degree_ratio;
    motor->base.buffer = 0;
    motor->base.degrees = 0;
    motor->base.reading = false;

    pinMode(interrupt_pin1, INPUT_PULLUP);
    pinMode(interrupt_pin2, INPUT_PULLUP);
    pinMode(pin1, OUTPUT);
    pinMode(pin2, OUTPUT);
    attachInterrupt(digitalPinToInterrupt(interrupt_pin1), interrupt_handler1,
                    CHANGE);
}

void advanced_motor_turn_to_degree(Advanced_Motor *motor, uint16_t degree)
{
    // Turn to degree should only accept a degree value between
    // 0 and 359 inclusive
    ASSERT(degree < 360);

    /*
  if (degree >= 360); 
  {
    Serial.print("advanced_motor_turn_to_degree was supplied ");
    Serial.print("with an invalid degree: ");
    Serial.println(degree);
    return;
  }
  */

    int32_t current_pos = advanced_motor_get_degrees(motor);
    int32_t turns = (current_pos / 360);

    // We need to choose two goal, one above and one below our current_pos.
    // These goal needs to be the full number representaion of the degree number
    // that has to be reached.
    int32_t goal2, goal1 = 360 * turns + (int32_t)degree;
    int32_t distance_forward, distance_backward;

    if (goal1 > current_pos)
    {
        goal2 = 360 * (turns - 1) + (int32_t)degree;
        distance_forward = current_pos - goal2;
        distance_backward = goal1 - current_pos;
    }
    else if (goal1 < current_pos)
    {
        goal2 = 360 * (turns + 1) + (int32_t)degree;
        distance_forward = current_pos - goal1;
        distance_backward = goal2 - current_pos;
    }
    // If goal and current_pos are equal, the motor doesn't need to turn, so
    // we just return
    else
    {
        return;
    }

    // Choose which direction to turn based on which distance is shortest
    if (distance_forward < distance_backward)
        advanced_motor_turn_degrees(motor, distance_forward, FORWARD);
    else
        advanced_motor_turn_degrees(motor, distance_backward, BACKWARD);
}

void motor_turn_to_degree(Motor *motor, uint16_t degree)
{
    // Turn to degree should only accept a degree value between
    // 0 and 359 inclusive
    ASSERT(degree < 360);

    /*
  if (degree >= 360) 
  {
    Serial.print("motor_turn_to_degree was supplied with an invalid degree: ");
    Serial.println(degree);
    return;
  }
  */

    int32_t current_pos = motor_get_degrees(motor);
    int32_t turns = (current_pos / 360);
    int32_t goal = 360 * turns + degree;

    // If goal is lower than current_pos, then we need to choose a new goal,
    // 360 degrees bigger
    if (goal < current_pos)
        goal = 360 * (turns + 1) + degree;

    // Turn motor by the distance to the goal
    motor_turn_degrees(motor, goal - current_pos);
}

void advanced_motor_turn_degrees(Advanced_Motor *motor, uint16_t degrees,
                                 Turning_Direction direction)
{
    // Calculate the goal that the motor should reach
    int32_t goal = advanced_motor_get_degrees(motor) + ((int32_t)degrees * direction);

    advanced_motor_turn(motor, direction);

        bool waiting = true;
        int32_t motor_degrees;
    // Wait for the motor to reach the goal
    switch (direction)
    {
    case FORWARD:
        //while (goal < advanced_motor_get_degrees(motor))
            ;
         while (waiting)
         {
             motor_degrees = advanced_motor_get_degrees(motor);

             DEBUG_PRINTLN_VAR(motor_degrees);
             DEBUG_PRINTLN_VAR(goal);
             waiting =  goal < motor_degrees;
             DEBUG_PRINTLN_VAR(goal < motor_degrees);
         }
        break;
    case BACKWARD:
        while (goal > advanced_motor_get_degrees(motor))
            ;
        break;
    }

    advanced_motor_stop(motor);
}

void motor_turn_degrees(Motor *motor, uint16_t degrees)
{
    // Calculate the goal that the motor should reach
    int32_t goal = motor_get_degrees(motor) + degrees;

    motor_turn(motor);

    // Wait for the motor to reach the goal
    while (goal > motor_get_degrees(motor))
        ;

    motor_stop(motor);
}

void advanced_motor_stop(Advanced_Motor *motor)
{
    // HACK: To break, we turn the motor in the other direction for a
    // fix amount of time.
    // It is not perfect, but it does reduce the coasting by a lot
    if (motor->direction == FORWARD)
        advanced_motor_turn(motor, BACKWARD);
    else
        advanced_motor_turn(motor, FORWARD);

    delay(50); // 50ms seems like a good delay
    digitalWrite(motor->pin1, LOW);
    digitalWrite(motor->pin2, LOW);
    delay(100); // give the motor time to stop coasting
}

void motor_stop(Motor *motor)
{
    digitalWrite(motor->pin, LOW);
}

void advanced_motor_turn(Advanced_Motor *motor, Turning_Direction direction)
{
    switch (direction)
    {
    case FORWARD:
        digitalWrite(motor->pin1, HIGH);
        digitalWrite(motor->pin2, LOW);
        break;
    case BACKWARD:
        digitalWrite(motor->pin1, LOW);
        digitalWrite(motor->pin2, HIGH);
        break;
    default:
        ASSERT(false);

        /*
      Serial.println("Not supported direction!");
      delay(1000);
      exit(0);
      */
        break;
    }
}

void advanced_motor_turn_analog(Advanced_Motor *motor,
                                Turning_Direction direction, uint8_t value)
{
    switch (direction)
    {
    case FORWARD:
        analogWrite(motor->pin1, value);
        digitalWrite(motor->pin2, LOW);
        break;
    case BACKWARD:
        digitalWrite(motor->pin1, LOW);
        analogWrite(motor->pin2, value);
        break;
    default:
        ASSERT(false);
        /*
      Serial.println("Not supported direction!");
      delay(1000);
      exit(0);
      */
        break;
    }
}

void motor_turn(Motor *motor)
{
    digitalWrite(motor->pin, HIGH);
}

void motor_turn_analog(Motor *motor, uint8_t value)
{
    analogWrite(motor->pin, value);
}

int32_t motor_get_degrees(Motor *motor)
{
    return base_motor_get_degrees(&motor->base);
}

int32_t advanced_motor_get_degrees(Advanced_Motor *motor)
{
    return base_motor_get_degrees(&motor->base);
}

void motor_update_degrees(Motor *motor)
{
    // If degrees are being read, update a buffer instead to avoid race condtions
    if (motor->base.reading)
    {
        motor->base.buffer++;
    }
    else
    {
        // We need to add and reset the buffer when we are allowed to
        // write to the degrees
        motor->base.degrees += 1 + motor->base.buffer;
        motor->base.buffer = 0;
    }
}

void advanced_motor_update_degrees(Advanced_Motor *motor)
{
    byte pattern = digitalRead(motor->interrupt_pin1);
    pattern |= (digitalRead(motor->interrupt_pin2) << 1);

    // We determin the direction by a pattern formed by the values
    // read from pin1 and pin2
    // LOW = 0, HIGH = 1
    switch (pattern)
    {
    case 0b11:
    case 0b00:
        motor->direction = FORWARD;
        break;
    case 0b10:
    case 0b01:
        motor->direction = BACKWARD;
        break;
    default:
        ASSERT(false);
        //Serial.println("Error in determin direction of motor");
        break;
    }

    // If degrees are being read, update a buffer instead to avoid race condtions
    if (motor->base.reading)
    {
        motor->base.buffer += motor->direction;
    }
    else
    {
        // We need to add and reset the buffer when we are allowed to
        // write to the degrees
        motor->base.degrees += motor->direction + motor->base.buffer;
        motor->base.buffer = 0;
    }
}
