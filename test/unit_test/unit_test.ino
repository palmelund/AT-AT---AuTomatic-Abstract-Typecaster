// Including everything, just in case
#include "unit_test.h"

void setup()
{
    Serial.begin(9600);
    while(!Serial) ;

    test_euclidean();
    test_determin_bounding_sphere();
}

void loop()
{
    
}

void print_rgb_delta(Delta_RGB* in)
{
    Serial.print("(");
    Serial.print(in->rgb.red);
    Serial.print(", ");
    Serial.print(in->rgb.green);
    Serial.print(", ");
    Serial.print(in->rgb.blue);
    Serial.print("), ");
    Serial.print(in->delta);
}

bool within_delta(double expected, double actual, double delta)
{
    return expected * (1.0 - delta) <= actual && expected * (1.0 + delta) >= actual;
}

void expect_rgb_delta(Delta_RGB* expected, Delta_RGB* actual)
{
    if (!within_delta(expected->rgb.red, actual->rgb.red, DBS_EXPECTED_DELTA) ||     
        !within_delta(expected->rgb.green, actual->rgb.green, DBS_EXPECTED_DELTA) || 
        !within_delta(expected->rgb.blue, actual->rgb.blue, DBS_EXPECTED_DELTA) ||   
        !within_delta(expected->delta, actual->delta, DBS_EXPECTED_DELTA))           
    {                                                           
        Serial.println("Failed");                               
        Serial.print("- Expected: ");                           
        print_rgb_delta(expected);                              
        Serial.print(" +/- ");
        Serial.println(DBS_EXPECTED_DELTA);                                       
                                                                
        Serial.print("- Actual: ");                             
        print_rgb_delta(actual);                                
        Serial.println();               
    }
    else
    {
        Serial.println("Success");
    }
}

#define ID 1
DBS_TEST_DECLARE_DATA(ID, 2) = {
    {         0,         0,         0 },
    { USHRT_MAX, USHRT_MAX, USHRT_MAX }
};
DBS_TEST_DECLARE_EXPECTED(ID) = {
    { 32767, 32767, 32767 }, 56754 // TODO: FIX
};

#define ID 2
DBS_TEST_DECLARE_DATA(ID, 2) = {
    {         0,        0, USHRT_MAX },
    { USHRT_MAX,        0,         0 }
};
DBS_TEST_DECLARE_EXPECTED(ID) = {
    { 32767, 0, 32767 }, 46340 
};

#define ID 3
DBS_TEST_DECLARE_DATA(ID, 2)  = {
    {         0, USHRT_MAX, USHRT_MAX },
    { USHRT_MAX, USHRT_MAX,         0 }
};
DBS_TEST_DECLARE_EXPECTED(ID) = {
    { 32767, 65535, 32767 }, 46340
};

#define ID 4
DBS_TEST_DECLARE_DATA(ID, 8)  = {
    { USHRT_MAX,         0,         0 }, // 1
    { USHRT_MAX, USHRT_MAX,         0 }, // 2
    { USHRT_MAX,         0, USHRT_MAX }, // 3
    { USHRT_MAX, USHRT_MAX, USHRT_MAX }, // 4
    {         0, USHRT_MAX,         0 }, // 5
    {         0, USHRT_MAX, USHRT_MAX }, // 6
    {         0,         0, USHRT_MAX }, // 7
    {         0,         0,         0 }  // 8
};
DBS_TEST_DECLARE_EXPECTED(ID) = {
    { 32767, 32767, 32767 }, 56754
};

#define ID 5
DBS_TEST_DECLARE_DATA(ID, 3)  = {
    {  500,  500,  500 },
    { 1000, 1200,  500 },
    { 1500,  500,  500 }
};
DBS_TEST_DECLARE_EXPECTED(ID) = {
    { 1000, 600, 500 }, 600
};

void test_determin_bounding_sphere()
{
    DBS_TEST_RUN(1);
    DBS_TEST_RUN(2);
    DBS_TEST_RUN(3);
    DBS_TEST_RUN(4);
    DBS_TEST_RUN(5);
}










void build_rgb_test(RGB_Test* t, 
    uint8_t r1, uint8_t g1, uint8_t b1, 
    uint8_t r2, uint8_t g2, uint8_t b2, 
    float e, float r)
{
    t->rgb1 = {r1, g1, b1};
    t->rgb2 = {r2, g2, b2};
    t->approx_expected = e;
    t->approx_range = r;
}

#define EUCLID_TEST_SIZE 6
void test_euclidean()
{
    RGB_Test euclidean_tests[EUCLID_TEST_SIZE];
    build_rgb_test(&euclidean_tests[0], 0, 0, 0, 0, 0, 0, 0, 0);
    build_rgb_test(&euclidean_tests[1], 1, 2, 3, 4, 5, 6, 5.1962, 0.01);
    build_rgb_test(&euclidean_tests[2], 10, 20, 30, 40, 50, 60, 51.962, 0.01);
    build_rgb_test(&euclidean_tests[3], 255, 255, 255, 0, 0, 0, 441.67, 0.01);
    build_rgb_test(&euclidean_tests[4], 0, 0, 0, 255, 255, 255, 441.67, 0.01);
    build_rgb_test(&euclidean_tests[5], 0, 127, 255, 255, 127, 0, 360.62, 0.01);

    for(int i = 0; i < EUCLID_TEST_SIZE; i++)
    {
        float res = euclidean_distance_3d(
            euclidean_tests[i].rgb1.red, euclidean_tests[i].rgb1.green, euclidean_tests[i].rgb1.blue,
            euclidean_tests[i].rgb2.red, euclidean_tests[i].rgb2.green, euclidean_tests[i].rgb2.blue);


        Serial.print("Euclidean Test: ");
        Serial.print(i);

        if (res <= euclidean_tests[i].approx_expected + euclidean_tests[i].approx_range && 
            res >= euclidean_tests[i].approx_expected - euclidean_tests[i].approx_range)
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