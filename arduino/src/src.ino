#include "defines.h"
#include "motor_api.h"
#include "distance_sensor_api.h"
#include "src.h"
#include "SparkFunISL29125.h"
#include "io_api.h"

SFE_ISL29125 RGB_sensor;
Motor motor_conveyor, motor_feeder;
Advanced_Motor adv_motor_separator;
Ultra_Sound_Sensor distance_sensor;
volatile bool running = true;
volatile bool stopped = false;
int32_t distance_to_wall;

RGB background_rgb;
RGB red_rgb;
RGB yellow_rgb;
RGB green_rgb;
RGB blue_rgb;

void setup() 
{
  Serial.begin(9600);
  while (!Serial);
  /*
  //exit(0);

  DEBUG_PRINTLN("Initializing all components...");
  DEBUG_PRINTLN("- Ultra sound sensor...");
  distance_sensor_init(&distance_sensor, RANGE_TRIG, RANGE_ECHO);

  DEBUG_PRINTLN("--- Calibrating...");
  distance_to_wall = calibrate_ultra_sound_sensor();
  DEBUG_PRINT("---"); DEBUG_PRINTLN_VAR(distance_to_wall);

  DEBUG_PRINTLN("- Motors...");
  motor_init(&motor_conveyor, 0.36, MOTOR_CONVEYOR_PIN, MOTOR_CONVEYOR_INT_PIN, 
    motor_conveyor_interrupt);
  motor_init(&motor_feeder, 1.0, MOTOR_FEEDER_PIN, MOTOR_FEEDER_INT_PIN, 
    motor_feeder_interrupt);

  advanced_motor_init(&adv_motor_separator, 1.0, MOTOR_SEPARATOR_PIN1, 
    MOTOR_SEPARATOR_PIN2,MOTOR_SEPARATOR_INT_PIN1, MOTOR_SEPARATOR_DATA_PIN, 
    adv_motor_separator_interrupt1);

  DEBUG_PRINTLN("Starting the sorting machine...");
  motor_turn_analog(&motor_conveyor, 255);
  */

  for (;;)
  {
    In_Message message_received;
    io_await_message(&message_received);
    
    switch(message_received.type)
    {
      case IN_MESSAGE_COLOR:
        Serial.print("Color: "); 
        Serial.println(message_received.color.type);

        Serial.print("Value: "); 
        Serial.println(message_received.color.value);
        break;

      case IN_MESSAGE_COMMAND:
        Serial.print("Command: "); 
        Serial.println(message_received.command.type);
        break;

      case IN_MESSAGE_DISTANCE:
        Serial.print("Distance: "); 
        Serial.println(message_received.distance.value);
        break;

      case IN_MESSAGE_OBJECT:
        Serial.print("Object: "); 
        Serial.println(message_received.object.type);
        break;

      default:
        break;
    }
  }

}

void motor_conveyor_interrupt() 
{
  motor_update_degrees(&motor_conveyor);
}

void motor_feeder_interrupt() 
{
  motor_update_degrees(&motor_feeder);
}

void adv_motor_separator_interrupt1() 
{
  advanced_motor_update_degrees(&adv_motor_separator);
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

// ----- Loop -----
void loop() 
{
  static int16_t bucket_pos [5] = { 0, 50, 100, 260, 310 };
  static Segment_Queue segment_queue;
  static Ball_Color last_ball = EMPTY;
  static int32_t conveyor_target = 90;

  Ball_Color read_color = EMPTY;
  int32_t test_dist = distance_sensor_measure_distance(&distance_sensor);

  DEBUG_PRINT_VAR(test_dist);
  DEBUG_PRINT(" ");
  DEBUG_PRINTLN_VAR(distance_to_wall);

  // Tests if a ball is in front of sensor
  if (test_dist < distance_to_wall)
  {
    read_color = read_color_sensor();
    DEBUG_PRINT("ball found: ");
    DEBUG_PRINTLN(get_color_name(read_color));
  }
  
  enqueue_segment(&segment_queue, read_color);
  feed_ball();

  Ball_Color current_ball = peek_segment(&segment_queue);

  // We only wonna move the buckets, if a ball was found, and that ball is
  // different from the last ball
  if (current_ball != EMPTY && current_ball != last_ball)
  {
    DEBUG_PRINT("ejecting: ");
    DEBUG_PRINTLN(get_color_name(current_ball));
    advanced_motor_turn_to_degree(&adv_motor_separator, 
      bucket_pos[current_ball]);

    last_ball = current_ball; 
  }

  while(motor_get_degrees(&motor_conveyor) < conveyor_target);
  conveyor_target += 90;
}

void feed_ball() 
{
  const uint8_t feed_iteration = 8;
  static uint8_t feed_counter = feed_iteration;
  static int16_t deg = 90;
  
  // We only feed a ball every x iterations
  if (feed_counter == feed_iteration) 
  {
    motor_turn_to_degree(&motor_feeder, deg);

    deg += 90;
    if (deg == 360) 
      deg = 0;

    feed_counter = 0;
  }
  
  feed_counter++;
}

Ball_Color read_color_sensor()
{
  static uint8_t next_ball = 0;
  Ball_Color res = (Ball_Color)next_ball;
  next_ball++;
  if (next_ball > 3)
    next_ball = 0;

  return res;
}

void enqueue_segment(Segment_Queue* segment_queue, Ball_Color segment)
{
  segment_queue->data[segment_queue->index] = segment;

  segment_queue->index++;
  if (segment_queue->index >= QUEUE_SIZE)
    segment_queue->index = 0;
}

Ball_Color peek_segment(Segment_Queue* segment_queue)
{
  return segment_queue->data[segment_queue->index];
}

char* get_color_name(Ball_Color color)
{
  switch(color)
  {
    case GREEN:
      return (char*)"Green";
    case YELLOW:
      return (char*)"Yellow";
    case RED:
      return (char*)"Red";
    case BLUE:
      return (char*)"Blue";
    case EMPTY:
      return (char*)"Empty";
    default:
      DEBUG_PRINT("Error in get_color_name");
      delay(1000);
      exit(0);
  }
}

void calibrate_color()
{
  uint16_t reading[10][3];

DEBUG_PRINTLN("Calibrating BACKGRROUND");
  for(int i = 0; i < 10; i++)
  {
    reading[i][0] = RGB_sensor.readRed();
    reading[i][1] = RGB_sensor.readGreen();
    reading[i][2] = RGB_sensor.readBlue();
  }

DEBUG_PRINTLN("Calibrating RED");

DEBUG_PRINTLN("Calibrating YELLOW");

DEBUG_PRINTLN("Calibrating GREEN");

DEBUG_PRINTLN("Calibrating BLUE");
}

RGB get_color_rgb()
{

}

Ball_Color get_color()
{
    uint16_t red = RGB_sensor.readRed();
    uint16_t green = RGB_sensor.readGreen();
    uint16_t blue = RGB_sensor.readBlue();
}