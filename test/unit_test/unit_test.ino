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

void dbs_run_test(uint8_t id, RGB* data, uint8_t count, Delta_RGB* expected)
{
    Delta_RGB actual;                                 
    determin_bounding_sphere(data, count, &actual);     
                                                                
    Serial.print("Determin bounding sphere ");                  
    Serial.print(id);                                           
    Serial.print(" - ");

    if (!within_delta(expected->rgb.red, actual.rgb.red, DBS_EXPECTED_DELTA) ||
        !within_delta(expected->rgb.green, actual.rgb.green, DBS_EXPECTED_DELTA) || 
        !within_delta(expected->rgb.blue, actual.rgb.blue, DBS_EXPECTED_DELTA) ||   
        !within_delta(expected->delta, actual.delta, DBS_EXPECTED_DELTA))           
    {
        Serial.println("Failed");                               
        Serial.print("- Expected: ");                           
        print_rgb_delta(expected);                              
        Serial.print(" +/- ");
        Serial.println(DBS_EXPECTED_DELTA);                                       
                                                                
        Serial.print("- Actual: ");                             
        print_rgb_delta(&actual);                                
        Serial.println();               
    }
    else
    {
        Serial.println("Success");
    }
}

void test_determin_bounding_sphere()
{
    // Test 1
    RGB data1[2] = {
        {         0,         0,         0 },
        { USHRT_MAX, USHRT_MAX, USHRT_MAX }
    };

    Delta_RGB expect1 = {
        { 32767, 32767, 32767 }, 56754
    };
    dbs_run_test(1, data1, sizeof(data1) / sizeof(RGB), &expect1);


    // Test 2
    RGB data2[2] = {
        {         0,        0, USHRT_MAX },
        { USHRT_MAX,        0,         0 }
    };

    Delta_RGB expect2 = {
        { 32767, 0, 32767 }, 46340 
    };
    dbs_run_test(2, data2, sizeof(data2) / sizeof(RGB), &expect2);


    // Test 3
    RGB data3[2] = {
        {         0, USHRT_MAX, USHRT_MAX },
        { USHRT_MAX, USHRT_MAX,         0 }
    };

    Delta_RGB expect3 = {
        { 32767, 65535, 32767 }, 46340 
    };
    dbs_run_test(3, data3, sizeof(data3) / sizeof(RGB), &expect3);


    // Test 4
    RGB data4[8] = {
        { USHRT_MAX,         0,         0 }, // 1
        { USHRT_MAX, USHRT_MAX,         0 }, // 2
        { USHRT_MAX,         0, USHRT_MAX }, // 3
        { USHRT_MAX, USHRT_MAX, USHRT_MAX }, // 4
        {         0, USHRT_MAX,         0 }, // 5
        {         0, USHRT_MAX, USHRT_MAX }, // 6
        {         0,         0, USHRT_MAX }, // 7
        {         0,         0,         0 }  // 8
    };

    Delta_RGB expect4 = {
        { 32767, 32767, 32767 }, 56754 
    };
    dbs_run_test(4, data4, sizeof(data4) / sizeof(RGB), &expect4);


    // Test 5
    RGB data5[3] = {
        {  500,  500,  500 },
        { 1000, 1200,  500 },
        { 1500,  500,  500 }
    };

    Delta_RGB expect5 = {
        { 1000, 600, 500 }, 600
    };
    dbs_run_test(5, data5, sizeof(data5) / sizeof(RGB), &expect5);
}










void build_rgb_test(RGB_Test* t, 
    float r1, float g1, float b1, 
    float r2, float g2, float b2, 
    float e)
{
    t->rgb1[0] = r1;
    t->rgb1[1] = g1; 
    t->rgb1[2] = b1;
    t->rgb2[0] = r2;
    t->rgb2[1] = g2; 
    t->rgb2[2] = b2;
    t->approx_expected = e;
}

#define EUCLID_TEST_SIZE 13
void test_euclidean()
{
    RGB_Test euclidean_tests[EUCLID_TEST_SIZE];
    build_rgb_test(&euclidean_tests[0], 
        0, 0, 0, 
        0, 0, 0, 
        0);
    build_rgb_test(&euclidean_tests[1], 
        0, 0, 0, 
        0, 0, USHRT_MAX, 
        USHRT_MAX);
    build_rgb_test(&euclidean_tests[2], 
        0, 0, 0, 
        0, USHRT_MAX, 0, 
        USHRT_MAX);
    build_rgb_test(&euclidean_tests[3], 
        0, 0, 0, 
        USHRT_MAX, 0, 0, 
        USHRT_MAX);
    build_rgb_test(&euclidean_tests[4], 
        0, 0, USHRT_MAX, 
        0, 0, 0, 
        USHRT_MAX);
    build_rgb_test(&euclidean_tests[5], 
        0, USHRT_MAX, 0, 
        0, 0, 0, 
        USHRT_MAX);
    build_rgb_test(&euclidean_tests[6], 
        USHRT_MAX, 0, 0, 
        0, 0, 0, 
        USHRT_MAX);

    build_rgb_test(&euclidean_tests[7], 
        USHRT_MAX, 0, 0, 
        0, 0, USHRT_MAX, 
        92680.4858101208);
    build_rgb_test(&euclidean_tests[8], 
        USHRT_MAX, 0, 0, 
        0, USHRT_MAX, 0, 
        92680.4858101208);
    build_rgb_test(&euclidean_tests[9], 
        USHRT_MAX, 0, 0, 
        USHRT_MAX, 0, 0, 
        0);
    build_rgb_test(&euclidean_tests[10], 
        USHRT_MAX, 0, USHRT_MAX, 
        0, 0, 0, 
        92680.4858101208);
    build_rgb_test(&euclidean_tests[11], 
        USHRT_MAX, USHRT_MAX, 0, 
        0, 0, 0, 
        92680.4858101208);

    build_rgb_test(&euclidean_tests[12], 
        0, 0, 0, 
        USHRT_MAX, USHRT_MAX, USHRT_MAX, 
        113509.949674026);

    for(int i = 0; i < EUCLID_TEST_SIZE; i++)
    {
        float res = euclidean_distance_3d(
            euclidean_tests[i].rgb1[0], euclidean_tests[i].rgb1[1], euclidean_tests[i].rgb1[2],
            euclidean_tests[i].rgb2[0], euclidean_tests[i].rgb2[1], euclidean_tests[i].rgb2[2]);


        Serial.print("Euclidean Test: ");
        Serial.print(i);

        if (within_delta(res, euclidean_tests[i].approx_expected, DBS_EXPECTED_DELTA))
        {
            Serial.println(" - Success");
        }
        else
        {   
            Serial.print(" - Failed - Expected: ");
            Serial.print(euclidean_tests[i].approx_expected);
            Serial.print(" +/- ");
            Serial.print(DBS_EXPECTED_DELTA);
            Serial.print(" but got result: ");
            Serial.println(res);
        }
    }
}