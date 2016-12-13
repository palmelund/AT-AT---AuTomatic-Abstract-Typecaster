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

    // Just to make sure motor->degrees never changes when reading = true
    ASSERT(res == motor->degrees);
    motor->reading = false;

    return res * motor->degree_ratio;
}

void base_motor_init(Base_Motor* motor, float degree_ratio,
                uint8_t interrupt_pin1, uint8_t interrupt_pin2, 
                void (*interrupt_handler)(void))
{
    motor->interrupt_pin1 = interrupt_pin1;
    motor->interrupt_pin2 = interrupt_pin2;
    motor->degree_ratio = degree_ratio;
    motor->buffer = 0;
    motor->degrees = 0;
    motor->reading = false;
    
    pinMode(interrupt_pin1, INPUT_PULLUP);
    pinMode(interrupt_pin2, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(interrupt_pin1), 
                    interrupt_handler, CHANGE);
}



void base_motor_update_degrees(Base_Motor *motor)
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
        break;
    }

    // If degrees are being read, update a buffer instead to avoid race condtions
    if (motor->reading)
    {
        motor->buffer += motor->direction;
    }
    else
    {
        // We need to add and reset the buffer when we are allowed to
        // write to the degrees
        motor->degrees += motor->direction + motor->buffer;
        motor->buffer = 0;
    }
}

/********************
 * Public functions *
 ********************/
void motor_init(Motor *motor, float degree_ratio, uint8_t pin,
                uint8_t interrupt_pin1, uint8_t interrupt_pin2, 
                void (*interrupt_handler)(void))
{
    base_motor_init(&motor->base, degree_ratio, interrupt_pin1, 
                    interrupt_pin2, interrupt_handler);

    motor->pin = pin;
    pinMode(pin, OUTPUT);
}

void advanced_motor_init(Advanced_Motor *motor, float degree_ratio,
                         uint8_t pin1, uint8_t pin2, uint8_t interrupt_pin1,
                         uint8_t interrupt_pin2,
                         void (*interrupt_handler)(void))
{
    base_motor_init(&motor->base, degree_ratio, interrupt_pin1, 
                    interrupt_pin2, interrupt_handler);

    motor->pin1 = pin1;
    motor->pin2 = pin2;

    pinMode(pin1, OUTPUT);
    pinMode(pin2, OUTPUT);
}

bool advanced_motor_turn_to_degree(Advanced_Motor *motor, uint16_t degree)
{
    // Turn to degree should only accept a degree value between
    // 0 and 359 inclusive
    ASSERT(degree < 360);

    int32_t current_pos = advanced_motor_get_degrees(motor);

    // We need to floor, because default casting rounds towards 0, and
    // we always wants it to round down for these calculations to work.
    int32_t turns = floor(current_pos / 360.0);

    // We need to choose two goal, one above and one below our current_pos.
    // These goal needs to be the full number representaion of the degree number
    // that has to be reached.
    int32_t goal2, goal1 = 360 * turns + degree;
    int32_t distance_forward, distance_backward;

    if (goal1 > current_pos)
    {
        goal2 = 360 * (turns - 1) + degree;
        distance_forward = current_pos - goal2;
        distance_backward = goal1 - current_pos;
    }
    else if (goal1 < current_pos)
    {
        goal2 = 360 * (turns + 1) + degree;
        distance_forward = current_pos - goal1;
        distance_backward = goal2 - current_pos;
    }
    // If goal and current_pos are equal, the motor doesn't need to turn, so
    // we just return
    else
    {
        return;
    }

    ASSERT(distance_forward >= 0);
    ASSERT(distance_backward >= 0);

    // Choose which direction to turn based on which distance is shortest
    if (distance_forward < distance_backward)
        return advanced_motor_turn_degrees(motor, distance_forward, FORWARD);
    else
        return advanced_motor_turn_degrees(motor, distance_backward, BACKWARD);
}

bool motor_turn_to_degree(Motor *motor, uint16_t degree)
{
    // Turn to degree should only accept a degree value between
    // 0 and 359 inclusive
    ASSERT(degree < 360);

    int32_t current_pos = motor_get_degrees(motor);
    int32_t turns = (current_pos / 360);
    int32_t goal = 360 * turns + degree;

    // If goal is lower than current_pos, then we need to choose a new goal,
    // 360 degrees bigger
    if (goal < current_pos)
        goal = 360 * (turns + 1) + degree;

    // Turn motor by the distance to the goal
    return motor_turn_degrees(motor, goal - current_pos);
}

bool lesser_than(int32_t value1, int32_t value2)
{
    return value1 < value2;
}

bool greater_than(int32_t value1, int32_t value2)
{
    return value1 > value2;
}

bool advanced_motor_turn_degrees(Advanced_Motor *motor, uint16_t degrees,
                                 int8_t direction)
{
    // Calculate the goal that the motor should reach
    int32_t read_degrees = advanced_motor_get_degrees(motor);
    int32_t goal = read_degrees + ((int32_t)degrees * direction);
    bool (*compare_to)(int32_t, int32_t);

    advanced_motor_turn(motor, direction);

    // Wait for the motor to reach the goal
    switch (direction)
    {
    case FORWARD:
        compare_to = lesser_than;
        break;
    case BACKWARD:
        compare_to = greater_than;
        break;
    default:
        ASSERT(false);
        break;
    }

    int32_t previouse_read_degrees;
    uint32_t error_timeout = millis() + 500;

    bool retval = true;
    do
    {
        previouse_read_degrees = read_degrees;
        read_degrees = advanced_motor_get_degrees(motor);

        if (previouse_read_degrees != read_degrees)
            error_timeout = millis() + 500;
        else if (error_timeout < millis())
        {
            retval = false;
            break;
        }
    } while (compare_to(goal, read_degrees));

    advanced_motor_stop(motor);
    return retval;
}

bool motor_turn_degrees(Motor *motor, uint16_t degrees)
{
    // Calculate the goal that the motor should reach
    int32_t read_degrees = motor_get_degrees(motor);
    int32_t goal = read_degrees + degrees;
    int32_t previouse_read_degrees;
    uint32_t error_timeout = millis() + 500;

    motor_turn(motor);

    bool retval = true;
    do
    {
        previouse_read_degrees = read_degrees;
        read_degrees = motor_get_degrees(motor);

        if (previouse_read_degrees != read_degrees)
            error_timeout = millis() + 500;
        else if (error_timeout < millis())
        {
            retval = false;
            break;
        }
    } while (goal > read_degrees);

    // Wait for the motor to reach the goal

    motor_stop(motor);
    return retval;
}

void advanced_motor_stop(Advanced_Motor *motor)
{
    // HACK: To break, we turn the motor in the other direction for a
    // fix amount of time.
    // It is not perfect, but it does reduce the coasting by a lot
    if (motor->base.direction == FORWARD)
        advanced_motor_turn(motor, BACKWARD);
    else
        advanced_motor_turn(motor, FORWARD);

    delay(70); // 60ms seems like a good delay
    digitalWrite(motor->pin1, LOW);
    digitalWrite(motor->pin2, LOW);
    delay(100); // give the motor time to stop coasting
}

void motor_stop(Motor *motor)
{
    digitalWrite(motor->pin, LOW);
}

void advanced_motor_turn(Advanced_Motor *motor, int8_t direction)
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
        break;
    }
}

void motor_turn(Motor *motor)
{
    digitalWrite(motor->pin, HIGH);
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
    base_motor_update_degrees(&motor->base);
}

void advanced_motor_update_degrees(Advanced_Motor *motor)
{
    base_motor_update_degrees(&motor->base);
}
