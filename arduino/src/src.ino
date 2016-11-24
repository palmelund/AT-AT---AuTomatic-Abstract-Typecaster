#include "src.h"

SFE_ISL29125 RGB_sensor;
Motor motor_conveyor, motor_feeder;
Advanced_Motor adv_motor_separator;
Ultra_Sound_Sensor distance_sensor;
volatile bool running = true;
volatile bool stopped = false;
int32_t distance_to_wall;

Delta_RGB background_rgb;
Delta_RGB red_rgb;
Delta_RGB yellow_rgb;
Delta_RGB green_rgb;
Delta_RGB blue_rgb;

void setup()
{
  Serial.begin(9600);
  while (!Serial)
    ;
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

    Out_Message message;
    switch (message_received.type)
    {
    case IN_MESSAGE_COLOR:
      message.type = OUT_MESSAGE_COLOR;
      message.color.type = message_received.color.type;
      message.color.value = message_received.color.value;
      io_send_message(&message);
      break;

    case IN_MESSAGE_COMMAND:
      message.type = OUT_MESSAGE_COMMAND;
      message.command.type = message_received.command.type;
      io_send_message(&message);
      break;

    case IN_MESSAGE_DISTANCE:
      message.type = OUT_MESSAGE_DISTANCE;
      message.distance.value = message_received.distance.value;
      io_send_message(&message);
      break;

    case IN_MESSAGE_OBJECT:
      message.type = OUT_MESSAGE_REQUEST;
      message.request.type = OUT_REQUEST_OBJECT_INFO;
      io_send_message(&message);

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
  static int16_t bucket_pos[5] = {0, 50, 100, 260, 310};
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

  while (motor_get_degrees(&motor_conveyor) < conveyor_target)
    ;
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

void enqueue_segment(Segment_Queue *segment_queue, Ball_Color segment)
{
  segment_queue->data[segment_queue->index] = segment;

  segment_queue->index++;
  if (segment_queue->index >= QUEUE_SIZE)
    segment_queue->index = 0;
}

Ball_Color peek_segment(Segment_Queue *segment_queue)
{
  return segment_queue->data[segment_queue->index];
}

char *get_color_name(Ball_Color color)
{
  switch (color)
  {
  case GREEN:
    return (char *)"Green";
  case YELLOW:
    return (char *)"Yellow";
  case RED:
    return (char *)"Red";
  case BLUE:
    return (char *)"Blue";
  case EMPTY:
    return (char *)"Empty";
  default:
    DEBUG_PRINT("Error in get_color_name");
    delay(1000);
    exit(0);
  }
}

Delta_RGB calibrate_color()
{
  RGB reading[CALIBRACTION_ITERATIONS];
  for (uint8_t i = 0; i < CALIBRACTION_ITERATIONS; i++)
  {
    reading[i] = get_color_rgb();
  }

  RGB point = reading[0];
  uint16_t greatest_distance = 0;
  uint8_t index = 0;

  for (uint8_t i = 1; i < CALIBRACTION_ITERATIONS; i++)
  {
    uint16_t tmp = euclidean_distance_3d(point, reading[i]);
    if (tmp > greatest_distance)
    {
      greatest_distance = tmp;
      index = i;
    }
  }

  uint16_t greatest_distance_new = 0;
  uint8_t index_new = 0;
  for (uint8_t i = 0; i < CALIBRACTION_ITERATIONS; i++)
  {
    uint16_t tmp = euclidean_distance_3d(reading[greatest_distance], reading[i]);
    if (tmp > greatest_distance_new)
    {
      greatest_distance_new = tmp;
      index_new = i;
    }
  }

  Delta_RGB midpoint;
  midpoint.rgb.red = (reading[index].red + reading[index_new].red) / 2;
  midpoint.rgb.green = (reading[index].green + reading[index_new].green) / 2;
  midpoint.rgb.blue = (reading[index].blue + reading[index_new].blue) / 2;
  midpoint.delta = greatest_distance_new / 2;

  for (;;)
  {
    uint8_t index_bigger = 0;
    bool outside = false;

    for (uint8_t i = 0; i < CALIBRACTION_ITERATIONS; i++)
    {
      if (euclidean_distance_3d(midpoint.rgb, reading[i]) > midpoint.delta)
      {
        outside = true;
        index_bigger = i;
        break;
      }
    }

    if (outside == true)
    {

      uint16_t euclidean_distanceance = euclidean_distance_3d(midpoint.rgb, reading[index_bigger]);
      uint16_t delta_red = (midpoint.rgb.red - reading[index_bigger].red) / euclidean_distanceance;
      uint16_t delta_green = (midpoint.rgb.green - reading[index_bigger].green) / euclidean_distanceance;
      uint16_t delta_blue = (midpoint.rgb.blue - reading[index_bigger].blue) / euclidean_distanceance;

      RGB new_point;
      new_point.red = midpoint.rgb.red + delta_red;
      new_point.green = midpoint.rgb.green + delta_green;
      new_point.blue = midpoint.rgb.blue + delta_blue; 

      midpoint.rgb = new_point;
      midpoint.delta = (euclidean_distanceance + midpoint.delta) / 2;
    }else{
      break;
    }
  }

  return midpoint;
}

uint16_t euclidean_distance_3d(RGB rgb1, RGB rgb2)
{
  int16_t res1 = (int16_t)rgb1.red - (int16_t)rgb2.red;
  int16_t res2 = (int16_t)rgb1.green - (int16_t)rgb2.green;
  int16_t res3 = (int16_t)rgb1.blue - (int16_t)rgb2.blue;

  return (uint16_t)sqrt(res1 * res1 + res2 * res2 + res3 * res3);
}

RGB get_color_rgb()
{
  RGB rgb = {RGB_sensor.readRed(), RGB_sensor.readGreen(), RGB_sensor.readBlue()};
  return rgb;
}

Ball_Color get_color()
{
  uint16_t red = RGB_sensor.readRed();
  uint16_t green = RGB_sensor.readGreen();
  uint16_t blue = RGB_sensor.readBlue();
}