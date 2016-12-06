// Including everything, just in case
#include <limits.h>
#include "defines.h"
#include "motor_api.h"
#include "distance_sensor_api.h"
#include "SparkFunISL29125.h"
#include "io_api.h"
#include "colors.h"
#include "segments.h"
#include "tasks.h"
#include <stdint.h>
#include "unit_test.h"

void setup()
{
    Serial.begin(9600);
    test_euclidean();
}

void loop()
{
    
}

void build_rgb_test(rgb_test* t, uint8_t r1, uint8_t g1, uint8_t b1, uint8_t r2, uint8_t g2, uint8_t b2, float e, float r)
{
    t->rgb1 = {r1, g1, b1};
    t->rgb2 = {r2, g2, b2};
    t->approx_expected = e;
    t->approx_range = r;
}

#define EUCLID_TEST_SIZE 6
void test_euclidean()
{
    rgb_test euclidean_tests[EUCLID_TEST_SIZE];
    build_rgb_test(&euclidean_tests[0], 0, 0, 0, 0, 0, 0, 0, 0);
    build_rgb_test(&euclidean_tests[1], 1, 2, 3, 4, 5, 6, 5.1962, 0.01);
    build_rgb_test(&euclidean_tests[2], 10, 20, 30, 40, 50, 60, 51.962, 0.01);
    build_rgb_test(&euclidean_tests[3], 255, 255, 255, 0, 0, 0, 441.67, 0.01);
    build_rgb_test(&euclidean_tests[4], 0, 0, 0, 255, 255, 255, 441.67, 0.01);
    build_rgb_test(&euclidean_tests[5], 0, 127, 255, 255, 127, 0, 360.62, 0.01);


    for(int i = 0; i < EUCLID_TEST_SIZE; i++ )
    {
        float res = euclidean_distance_3d(&(euclidean_tests[i].rgb1), &(euclidean_tests[i].rgb2));

            Serial.print("Euclidean Test: ");
            Serial.print(i);
        if (res <= euclidean_tests[i].approx_expected + euclidean_tests[i].approx_range && res >= euclidean_tests[i].approx_expected - euclidean_tests[i].approx_range)
        {
            Serial.println(" - Success");
        }
        else
        {
            Serial.print(" - Failed - Expected: ");
            Serial.print(euclidean_tests[i].approx_expected);
            Serial.print(" +/- ");
            Serial.print(euclidean_tests[i].approx_range);
            Serial.print(" but got result: ");
            Serial.println(res);
        }
    }
}