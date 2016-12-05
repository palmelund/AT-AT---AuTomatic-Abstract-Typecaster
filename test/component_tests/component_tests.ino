#include "defines.h"

#include "component_tests.h"
#include "motor_api.h"

void setup()
{

  Serial.begin(9600);
  DEBUG_PRINTLN("Debugging ON");

#ifdef COMPONENT_TEST_MOTOR_COAST
  Serial.println("Component Test: Motor Coast");
  component_test_motor_coast(&motor_conveyor);
#endif

#ifdef COMPONENT_TEST_MOTOR_TIME
  Serial.println("Component Test: Motor Time");
  component_test_motor_time(&motor_conveyor);
#endif

#ifdef COMPONENT_TEST_INTERRUPT_COST
  Serial.println("Component Test: Interupt Cost");
  component_test_interupt_cost(&adv_motor_separator);
#endif

#ifdef COMPONENT_TEST_MOTOR_DEGREE
  Serial.println("Component Test: Motor Degree");
  component_test_motor_degree(&adv_motor_separator);
#endif

#ifdef COMPONENT_TEST_COLOR
  Serial.println("Component Test: Color");
  component_test_color();
#endif

//#ifdef API_TEST_MOTOR_TURN_DEG
//  Serial.println("API Test: Test Motor Deg");
//  api_test_motor_turn_deg();
//#endif
}

void loop()
{

}