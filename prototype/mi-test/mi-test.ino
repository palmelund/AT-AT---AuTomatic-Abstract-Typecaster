#include "defines.h"
#include "motor_api.h"
#include "distance_sensor_api.h"

Motor motor_conveyor, motor_feeder;
Ultra_Sound_Sensor distance_sensor;

int32_t distance_to_wall;

void setup()
{
    Serial.begin(9600);

    motor_init(&motor_conveyor, 0.36, MOTOR_CONVEYOR_PIN, 
        MOTOR_CONVEYOR_INT_PIN, motor_conveyor_interrupt);
    motor_init(&motor_feeder, 1.0, MOTOR_FEEDER_PIN, MOTOR_FEEDER_INT_PIN, 
        motor_feeder_interrupt);
    distance_sensor_init(&distance_sensor, RANGE_TRIG, RANGE_ECHO);
    
    distance_to_wall = calibrate_ultra_sound_sensor();
    delay(1000);
    Serial.print("Distance to wall: ");
    Serial.println(distance_to_wall);
    delay(2500);
    motor_turn(&motor_conveyor);
    feed_ball();
}

void loop()
{
    //int32_t test_dist = distance_sensor_double_take(); //distance_sensor_measure_distance(&distance_sensor);
    //Serial.println(test_dist);

/*
    if(test_dist + 10 < distance_to_wall)
    {
        Serial.print("Log: ");
        Serial.println(millis());
    }

    static uint32_t time = millis();
    if (time + 5000 < millis())
    {
        feed_ball();
        time = millis();
    }

    return;
    */

    uint32_t pos = motor_get_degrees(&motor_conveyor);
    // Change TARGET1 for delay before requesting image
    while(pos + 360 > motor_get_degrees(&motor_conveyor)) {}
    Serial.print("Command:");
    Serial.print(2);
    Serial.println("end");
    // Change TARGET2 for delay after requesting image
    while(pos + 720 > motor_get_degrees(&motor_conveyor)) {}
    feed_ball();
}

void motor_conveyor_interrupt()
{
    motor_update_degrees(&motor_conveyor);
}

void motor_feeder_interrupt()
{
    motor_update_degrees(&motor_feeder);
}

void feed_ball() 
{
  static int16_t deg = 90;
  
    motor_turn_to_degree(&motor_feeder, deg);

    deg += 90;
    if (deg == 360) 
        deg = 0;
}

int32_t calibrate_ultra_sound_sensor()
{
  int32_t min = distance_sensor_measure_distance(&distance_sensor);
  int32_t current;
  for (uint8_t i = 0; i < CALIBRACTION_ITERATIONS - 1; i++)
  {
    DEBUG_PRINTLN_VAR(min);
    current = distance_sensor_measure_distance(&distance_sensor);

    if (current < min)
      min = current;
  }

  return min;
}

uint32_t distance_sensor_double_take()
{
    uint32_t reading_1 = distance_sensor_measure_distance(&distance_sensor);
    uint32_t reading_2 = distance_sensor_measure_distance(&distance_sensor);
    if (reading_1 == reading_2)
    {
        return reading_1;
    }
    else
    {
        return INT16_MAX;
    }
}