#pragma once

struct rgb_test
{
    RGB rgb1;
    RGB rgb2;
    float approx_expected;
    float approx_range;
};

void build_rgb_test(rgb_test* t, uint8_t r1, uint8_t g1, uint8_t b1, uint8_t r2, uint8_t g2, uint8_t b2, float e, float r);
void test_euclidean();