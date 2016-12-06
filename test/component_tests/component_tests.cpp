#include "component_tests.h"

const int16_t test_count = 20;

#ifdef COMPONENT_TEST_MOTOR_COAST
void component_test_motor_coast(Motor* motor) 
{
  for (int16_t i = 0; i < test_count; ++i) 
  {
    motor->base.degrees = 0;
    motor_turn_degrees(motor, 360);

    delay(1000);

    // Value used for component testing.
    // Should be 360 + coast degrees for the motor.
    Serial.println(motor_get_degrees(motor));
    
    delay(500);                               
  }
}
#endif

#ifdef COMPONENT_TEST_MOTOR_TIME
void component_test_motor_time(Motor* motor) 
{
  for (int16_t i = 0; i < test_count; ++i) 
  {
    motor->degrees = 0;
    unsigned int32_t t_start = millis();
    motor_turn_degrees(motor, 360);
    unsigned int32_t t_end = millis();

    Serial.println(t_end - t_start);
    delay(1000);
  }
}
#endif

#ifdef COMPONENT_TEST_INTERRUPT_COST
int32_t power_function(int32_t number, int32_t exponent)
{
  if (exponent == 0)
    return 1;
int1
  int32_t res = number;
  
  for (; exponent > 1; exponent--){
    res *= number; 
  }

  return res;
}

void print_formatted_number(int32_t number, int16_t total_size)
{
  bool leading_zeros = true;
  
  for (; total_size > 0; total_size--)
  {
    int32_t power = power_function(10, total_size - 1);
    int32_t digit = number / power;

    if (leading_zeros && digit != 0)
      leading_zeros = false;

    if (leading_zeros)
      Serial.print(" ");
    else
      Serial.print(digit);
    
    number -= digit * power;
  }
}

void print_table_row(int32_t with, int32_t without, int32_t delta, 
  int32_t count, double est_cost, double est_cost_percent)
{
  Serial.print("| ");
  print_formatted_number(with, 10);
  Serial.print(" | ");
  print_formatted_number(without, 10);
  Serial.print(" | ");
  print_formatted_number(delta, 10);
  Serial.print(" | ");
  print_formatted_number(count, 10);
  Serial.print(" | ");
  Serial.print(est_cost, 7);
  Serial.print(" | ");
  Serial.print(est_cost_percent, 7);
  Serial.println("% |");
}


void component_test_interupt_cost(Advanced_Motor* motor)
{
  const int32_t moter_wait = 1500;
  const int32_t iterations = 100000;

  int32_t time_start, time_end, interrupt_count_start, interrupt_count_end, 
       no_interrupt_time, interrupt_time, interrupt_count, time_delta,
       average_no_interrupt_time = 0, average_interrupt_time = 0, 
       average_interrupt_count = 0;
  
  Serial.print("| with       | without    | delta      | count      ");
  Serial.println("| est cost   | est cost % |");

  for (int16_t i = 0; i < test_count + 1; ++i)
  {
    time_start = micros();
    for (volatile int32_t j = 0; j < iterations; j++);
    time_end = micros();
  
    no_interrupt_time = time_end - time_start;


    advanced_motor_turn(motor, BACKWARD);
    delay(moter_wait);
    
    interrupt_count_start = advanced_motor_get_degrees(motor);
    time_start = micros();
    for (volatile int32_t j = 0; j < iterations; j++);
    time_end = micros();
    interrupt_count_end = advanced_motor_get_degrees(motor);;

    interrupt_time = time_end - time_start;
    interrupt_count = interrupt_count_end - interrupt_count_start;
    
    advanced_motor_stop(motor);

    if (i != 0)
    {
      time_delta = interrupt_time - no_interrupt_time;

      print_table_row(interrupt_time, no_interrupt_time, time_delta, 
        interrupt_count, time_delta / (double)interrupt_count, 
        time_delta / (double)interrupt_time);

      average_interrupt_time += interrupt_time;
      average_no_interrupt_time += no_interrupt_time;
      average_interrupt_count += interrupt_count;
    }
    
    delay(moter_wait);
  }

  average_no_interrupt_time /= test_count;
  average_interrupt_time /= test_count;
  average_interrupt_count /= test_count; 
  time_delta = average_interrupt_time - average_no_interrupt_time;
  
  Serial.print("-----------------------------------------");
  Serial.println("--------------------------------------");
  print_table_row(average_interrupt_time, average_no_interrupt_time, time_delta, 
    average_interrupt_count, time_delta / (double)average_interrupt_count, 
    time_delta / (double)average_interrupt_time);

  return;
  /*
  int32_t time_start, time_end, interrupt_count_start, interrupt_count_end;
  int32_t no_interrupt_times[test_count], interrupt_times[test_count], interrupt_counts[test_count];
  
  delay(2500); //HACK: this makes it so micros() gives us currect result on first iteration for some reason...
  // run measurements without a motor running
  for (int16_t i = 0; i < test_count; ++i){  
    time_start = micros();
    for (volatile int32_t j = 0; j < iterations; j++);
    time_end = micros();
  
    no_interrupt_times[i] = time_end - time_start;
  }
  // run measurements with a motor running
  motor_turn(&motor1, COUNTERCLOCKWISE);
  delay(2500);
  for (int16_t i = 0; i < test_count; ++i){
    interrupt_count_start = motor1.deg;
    time_start = micros();
    for (volatile int32_t j = 0; j < iterations; j++);
    time_end = micros();
    interrupt_count_end = motor1.deg;

    interrupt_times[i] = time_end - time_start;
    interrupt_counts[i] = interrupt_count_end - interrupt_count_start;
  }
    
  motor_stop(&motor1);
  
  int32_t average_no_interrupt_time = 0, average_interrupt_time = 0, average_interrupt_count = 0, delta;

  // print16_t result table
  Serial.println("| with       | without    | delta      | count      | est cost   | est cost % |");
  for (int16_t i = 0; i < test_count; ++i){
    delta = interrupt_times[i] - no_interrupt_times[i];
    print_table_row(interrupt_times[i], no_interrupt_times[i], delta, interrupt_counts[i],
                    delta / (double)interrupt_counts[i], delta / (double)interrupt_times[i]);

    average_interrupt_time += interrupt_times[i];
    average_no_interrupt_time += no_interrupt_times[i];
    average_interrupt_count += interrupt_counts[i];
  }

  average_no_interrupt_time /= test_count;
  average_interrupt_time /= test_count;
  average_interrupt_count /= test_count; 
  delta = average_interrupt_time - average_no_interrupt_time;
  
  Serial.println("-------------------------------------------------------------------------------");
  print_table_row(average_interrupt_time, average_no_interrupt_time, delta, average_interrupt_count,
                  delta / (double)average_interrupt_count, delta / (double)average_interrupt_time);
  */
}
#endif

#ifdef COMPONENT_TEST_MOTOR_DEGREE

#endif

#ifdef COMPONENT_TEST_COLOR

SFE_ISL29125 RGB_sensor;

void component_test_color()
{
RGB_sensor.init();
while (RGB_sensor.readRed() == 0 || RGB_sensor.readGreen() == 0 || RGB_sensor.readBlue() == 0) {}

  while(true)
  {
    while(Serial.available() < 1) {}
    Serial.read();
      
    Serial.println("Reading...");

      uint16_t rgb[3];

      rgb[0] = RGB_sensor.readRed();
      rgb[1] = RGB_sensor.readGreen();
      rgb[2] = RGB_sensor.readBlue();

      Serial.print(rgb[0]);
      Serial.print(" ");
      Serial.print(rgb[1]);
      Serial.print(" ");
      Serial.println(rgb[2]);
      
      Serial.println("Calibrating...");

      Delta_RGB drgb;
      calibrate_color(&RGB_sensor, &drgb);
      Serial.print(drgb.rgb.red);
      Serial.print(" ");
      Serial.print(drgb.rgb.green);
      Serial.print(" ");
      Serial.print(drgb.rgb.blue);
      Serial.print(" ");
      Serial.println(drgb.delta);
  }
}

#endif