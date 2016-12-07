#include "colors.h"

char *get_color_name(uint8_t color)
{
    switch (color)
    {
    case GREEN:
        return (char *)"G";
    case YELLOW:
        return (char *)"Y";
    case RED:
        return (char *)"R";
    case BLUE:
        return (char *)"B";
    case UNKNOWN:
        return (char *)"?";
    default:
        ASSERT(false);
    }
}

void read_color(SFE_ISL29125* RGB_sensor, RGB *result)
{
    result->red = RGB_sensor->readRed();
    result->green = RGB_sensor->readGreen();
    result->blue = RGB_sensor->readBlue();
}

float euclidean_distance_3d(RGB *rgb1, RGB *rgb2)
{
    int16_t res1 = (int16_t)rgb1->red - (int16_t)rgb2->red;
    int16_t res2 = (int16_t)rgb1->green - (int16_t)rgb2->green;
    int16_t res3 = (int16_t)rgb1->blue - (int16_t)rgb2->blue;

    float res = (float)sqrt(pow(res1, 2) + pow(res2, 2) + pow(res3, 2));
    
    //DEBUG_PRINT_RGB((*rgb1));
    //DEBUG_PRINT_RGB((*rgb2));
    //DEBUG_PRINT_VAR(res);
    
    return res;
}

void calibrate_color(SFE_ISL29125* RGB_sensor, Delta_RGB *result)
{
    // Read samples to calibrate from
    RGB samples[CALIBRACTION_ITERATIONS];
    for (uint8_t i = 0; i < CALIBRACTION_ITERATIONS; ++i)
    {
        read_color(RGB_sensor, &samples[i]);
        //DEBUG_PRINT_RGB(samples[i]);
    }

/*
    for (auto c : samples)
    {
        DEBUG_PRINT_RGB(c);
    }
*/
    // https://en.wikipedia.org/wiki/Bounding_sphere
    // Using Ritter's bounding sphere method.
    // We start by picking a point in 3d space
    RGB *point1 = &samples[0];
    RGB *point2;

    float greatest_distance = -1;

    // Find the point furthest away from point1
    for (uint8_t i = 1; i < CALIBRACTION_ITERATIONS; ++i)
    {
        float distance = euclidean_distance_3d(point1, &samples[i]);
        if (distance > greatest_distance)
        {
            greatest_distance = distance;
            point2 = &samples[i];
        }
    }

    greatest_distance = -1;

    // Find the point furthest away from point2
    for (uint8_t i = 0; i < CALIBRACTION_ITERATIONS; ++i)
    {
        float distance = euclidean_distance_3d(point2, &samples[i]);
        if (distance > greatest_distance)
        {
            greatest_distance = distance;
            point1 = &samples[i];
        }
    }

    // We now construct a sphere which cotains the two points.
    // This is done by first, finding the point in between the two points
    result->rgb.red = (point1->red + point2->red) / 2;
    result->rgb.green = (point1->green + point2->green) / 2;
    result->rgb.blue = (point1->blue + point2->blue) / 2;

    // Then finding the radius of the sphere, by taking the distance
    // from the center to one of the two points.
    // NOTE: We calculate both, since rounding errors gives us a point
    //       not exactly in the center
    float distance_to_point1 = euclidean_distance_3d(&result->rgb, point1);
    float distance_to_point2 = euclidean_distance_3d(&result->rgb, point2);

    // Choose the greatest radius, ensures that both points are in the sphere
    if (distance_to_point1 > distance_to_point2)
        result->delta = (uint16_t)ceil(distance_to_point1);
    else
        result->delta = (uint16_t)ceil(distance_to_point2);

    ASSERT(result->delta >= distance_to_point1);
    ASSERT(result->delta >= distance_to_point2);

    // Now we need to ensure that the sphere we just found actually
    // contains all points
    for (;;)    
    {
        RGB *outside_point = NULL;
        float distance_to_outside_point;

        // Checking if all points are contained
        for (uint8_t i = 0; i < CALIBRACTION_ITERATIONS; ++i)
        {
            distance_to_outside_point = 
                euclidean_distance_3d(&result->rgb, &samples[i]);
            if (distance_to_outside_point > (float)result->delta)
            {
                outside_point = &samples[i];
                break;
            }
        }

        // If no points was outside the sphere, we know the sphere contains
        // all points, and the algorithm is done
        if (outside_point == NULL)
            break;

        // When a point is found outside the sphere, we need a new sphere
        // that contains all points in the previous sphere + the outside point.

        // We construct the sphere by extending the line from the center
        // of the sphere to the outside point by the radius of the sphere.
        // NOTE: http://math.stackexchange.com/questions/352828/increase-length-of-line
        float delta_red = ((float)result->rgb.red - outside_point->red) 
            / distance_to_outside_point;
        float delta_green = ((float)result->rgb.green - outside_point->green) 
            / distance_to_outside_point;
        float delta_blue = ((float)result->rgb.blue - outside_point->blue) 
            / distance_to_outside_point;

        // The moved_point, will be the new point that is created from extending
        // that line.
        RGB moved_result;
        moved_result.red = 
            (uint16_t)ceil(result->rgb.red + (result->delta * delta_red));
        moved_result.green = 
            (uint16_t)ceil(result->rgb.green + (result->delta * delta_green));
        moved_result.blue = 
            (uint16_t)ceil(result->rgb.blue + (result->delta * delta_blue));

        // We now construct the new sphere which cotains outside_point and
        // moved_result. This is the same as how we created our original sphere.
        result->rgb.red = (moved_result.red + outside_point->red) / 2;
        result->rgb.green = (moved_result.green + outside_point->green) / 2;
        result->rgb.blue = (moved_result.blue + outside_point->blue) / 2;

        uint16_t prev_delta = result->delta;
        float distance_to_moved_result = 
            euclidean_distance_3d(&result->rgb, &moved_result);
        distance_to_outside_point = 
            euclidean_distance_3d(&result->rgb, outside_point);

        if (distance_to_moved_result > distance_to_outside_point)
            result->delta = (uint16_t)ceil(distance_to_moved_result);
        else
            result->delta = (uint16_t)ceil(distance_to_outside_point);

        ASSERT(result->delta >= distance_to_moved_result);
        ASSERT(result->delta >= distance_to_outside_point);

        DEBUG_PRINTLN_VAR(result->delta);
        DEBUG_PRINTLN_VAR(prev_delta);
        DEBUG_PRINTLN("");
        ASSERT(result->delta >= prev_delta);

        // Now we repeat!
    }

    // Just to add some extra in case of noise
    result->delta += 200;
}

uint8_t determin_color(Delta_RGB* known_colors, RGB *color)
{
    uint8_t closest_color = UNKNOWN;
    uint16_t closest_distance = USHRT_MAX;
    for (uint8_t i = 0; i < COLOR_COUNT; ++i)
    {
        uint16_t distance = euclidean_distance_3d(color, &known_colors[i].rgb);
        if (known_colors[i].delta > distance && closest_distance > distance)
        {
            closest_distance = distance;
            closest_color = i;
        }
    }

    return closest_color;
}