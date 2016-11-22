#include "SparkFunISL29125.h"

SFE_ISL29125 RGB_sensor;

void setup()
{
    Serial.begin(9600);
    if (RGB_sensor.init())
    {
        Serial.println("Sensor Initialization Successful\n");
    }
}

void loop()
{
    uint16_t red = RGB_sensor.readRed();
    uint16_t green = RGB_sensor.readGreen();
    uint16_t blue = RGB_sensor.readBlue();
  
    // Print out readings, change HEX to DEC if you prefer decimal output
    Serial.print("Red: "); Serial.println(red);
    Serial.print("Green: "); Serial.println(green);
    Serial.print("Blue: "); Serial.println(blue);
    Serial.println();
    delay(2000);
}