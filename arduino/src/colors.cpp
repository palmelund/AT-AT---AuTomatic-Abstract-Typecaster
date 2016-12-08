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

float euclidean_distance_3d(
    float x1, float y1, float z1,
    float x2, float y2, float z2)
{
    float res1 = x1 - x2;
    float res2 = y1 - y2;
    float res3 = z1 - z2;

    float res = sqrt(pow(res1, 2) + pow(res2, 2) + pow(res3, 2));
    
    //DEBUG_PRINT_RGB((*rgb1));
    //DEBUG_PRINT_RGB((*rgb2));
    //DEBUG_PRINT_VAR(res);
    
    return res;
}

void determin_bounding_sphere(RGB* samples, uint8_t sample_count, 
    Delta_RGB *result)
{
    ASSERT(sample_count > 1);
    
    /*
    for (uint8_t i = 0; i < sample_count; ++i)
    {
        DEBUG_PRINT_RGB(samples[i]);
    }
    */

    // https://en.wikipedia.org/wiki/Bounding_sphere
    // Using Ritter's bounding sphere method.
    // We start by picking a point in 3d space
    RGB *point1 = &samples[0];
    RGB *point2;

    float greatest_distance = -1;

    // Find the point furthest away from point1
    for (uint8_t i = 1; i < sample_count; ++i)
    {
        float distance = euclidean_distance_3d(
            point1->red, point1->green, point1->blue, 
            samples[i].red, samples[i].green, samples[i].blue);
        if (distance > greatest_distance)
        {
            greatest_distance = distance;
            point2 = &samples[i];
        }
    }

    ASSERT(greatest_distance >= 0);

    greatest_distance = -1;

    // Find the point furthest away from point2
    for (uint8_t i = 0; i < sample_count; ++i)
    {
        float distance = euclidean_distance_3d(
            point2->red, point2->green, point2->blue, 
            samples[i].red, samples[i].green, samples[i].blue);
        if (distance > greatest_distance)
        {
            greatest_distance = distance;
            point1 = &samples[i];
        }
    }

    ASSERT(greatest_distance >= 0);

    //DEBUG_PRINTLN_VAR(greatest_distance);
    //DEBUG_PRINT_RGB((*point1));
    //DEBUG_PRINT_RGB((*point2));

    // We now construct a sphere which cotains the two points.
    // This is done by first, finding the point in between the two points
    float result_red = ((float)point1->red + point2->red) / 2;
    float result_green = ((float)point1->green + point2->green) / 2;
    float result_blue = ((float)point1->blue + point2->blue) / 2;

    // Then finding the radius of the sphere, by taking the distance
    // from the center to one of the two points.
    // NOTE: We calculate both, since rounding errors gives us a point
    //       not exactly in the center
    float distance_to_point1 = euclidean_distance_3d(
         result_red, result_green, result_blue, 
         point1->red, point1->green, point1->blue);
    float distance_to_point2 = euclidean_distance_3d(
         result_red, result_green, result_blue, 
         point2->red, point2->green, point2->blue);
    float result_delta;

    // Choose the greatest radius, ensures that both points are in the sphere
    if (distance_to_point1 > distance_to_point2)
        result_delta = distance_to_point1;
    else
        result_delta = distance_to_point2;

    ASSERT(result_delta >= distance_to_point1);
    ASSERT(result_delta >= distance_to_point2);

    // Now we need to ensure that the sphere we just found actually
    // contains all points
    for (;;)    
    {
        RGB *outside_point = NULL;
        float distance_to_outside_point;

        // Checking if all points are contained
        for (uint8_t i = 0; i < sample_count; ++i)
        {
            distance_to_outside_point = euclidean_distance_3d(
                result_red, result_green, result_blue, 
                samples[i].red, samples[i].green, samples[i].blue);
            if (distance_to_outside_point > result_delta)
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
        float delta_red = (result_red - outside_point->red) 
            / distance_to_outside_point;
        float delta_green = (result_green - outside_point->green) 
            / distance_to_outside_point;
        float delta_blue = (result_blue - outside_point->blue) 
            / distance_to_outside_point;

        // The edge, will be the new point that is created from extending
        // that line.
        float edge_red = result_red + (result_delta * delta_red);
        float edge_green = result_green + (result_delta * delta_green);
        float edge_blue = result_blue + (result_delta * delta_blue);

        // We now construct the new sphere which cotains outside_point and
        // edge. This is the same as how we created our original sphere.
        result_red = (edge_red + outside_point->red) / 2;
        result_green = (edge_green + outside_point->green) / 2;
        result_blue = (edge_blue + outside_point->blue) / 2;

        float distance_to_moved_result = 
            euclidean_distance_3d(
                result_red, result_green, result_blue, 
                edge_red, edge_green, edge_blue);
        distance_to_outside_point = 
            euclidean_distance_3d(
                result_red, result_green, result_blue, 
                outside_point->red, outside_point->green, outside_point->blue);
        float prev_delta = result_delta;

        if (distance_to_moved_result > distance_to_outside_point)
            result_delta = distance_to_moved_result;
        else
            result_delta = distance_to_outside_point;

        ASSERT(result_delta >= distance_to_moved_result);
        ASSERT(result_delta >= distance_to_outside_point);

        DEBUG_PRINTLN_VAR(result_delta);
        DEBUG_PRINTLN_VAR(prev_delta);
        DEBUG_PRINTLN("");
        ASSERT(result_delta >= prev_delta);

        // Now we repeat!
    }

    result->rgb.red = (uint16_t)result_red;
    result->rgb.green = (uint16_t)result_green;
    result->rgb.blue = (uint16_t)result_blue;
    result->delta = (uint16_t)ceil(result_delta);
}

uint8_t determin_color(Delta_RGB* known_colors, RGB *color)
{
    uint8_t closest_color = UNKNOWN;
    uint16_t closest_distance = USHRT_MAX;
    for (uint8_t i = 0; i < COLOR_COUNT; ++i)
    {
        uint16_t distance = euclidean_distance_3d(
            color->red, color->green, color->blue, known_colors[i].rgb.red, 
            known_colors[i].rgb.green, known_colors[i].rgb.blue);
        if (known_colors[i].delta > distance && closest_distance > distance)
        {
            closest_distance = distance;
            closest_color = i;
        }
    }

    return closest_color;
}