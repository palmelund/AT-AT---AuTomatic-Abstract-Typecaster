#include <stdio.h>

#define COLORS 8
typedef enum 
{
    RED     = 0,
    GREEN   = 1,
    BLUE    = 2,
    YELLOW  = 3,
    PINK    = 4,
    CYAN    = 5,
    WHITE   = 6,
    BLACK   = 7,
} Color;

short colors[COLORS][3]
{
    { 255,   0,   0 }, // RED
    {   0, 255,   0 }, // GREEN
    {   0,   0, 255 }, // BLUE
    { 255, 255,   0 }, // YELLOW
    { 255,   0, 255 }, // PINK
    {   0, 255, 255 }, // CYAN
    { 255, 255, 255 }, // WHITE
    {   0,   0,   0 }, // BLACK
};

#define ifprint(c, s) case c: printf(#s); break;

void print_color(Color color) 
{
    switch (color) {
        ifprint(RED,    Red);
        ifprint(GREEN,  Green);
        ifprint(BLUE,   Blue);
        ifprint(YELLOW, Yellow);
        ifprint(PINK,   Pink);
        ifprint(CYAN,   Cyan);
        ifprint(WHITE,  White);
        ifprint(BLACK,  Black);
    }
}

int isqrt(int number) 
{
    int res = 0;
    int bit = 1 << 30; // The second-to-top bit is set: 1 << 30 for 32 bits
 
    // "bit" starts at the highest power of four <= the argument.
    while (bit > number)
        bit >>= 2;
        
    while (bit != 0) 
    {
        if (number >= res + bit) 
        {
            number -= res + bit;
            res = (res >> 1) + bit;
        }
        else
        {
            res >>= 1;
        }

        bit >>= 2;
    }
    
    return res;
}

int euclidean_distanceance_3d(short* vec1, short* vec2) 
{
    int res1 = vec1[0] - vec2[0];
    int res2 = vec1[1] - vec2[1];
    int res3 = vec1[2] - vec2[2];

    return isqrt(res1*res1 + res2*res2 + res3*res3);
}

Color determin_color(short* rgb) 
{
    int i, min = 2147483647;
    Color res;

    for (i = 0; i < COLORS; i++) 
    {
        int distance = euclidean_distanceance_3d(colors[i], rgb);
        
        if (distance < min) {
            res = i;
            min = distance;
        }
    }

    return res;
}

#define determin_and_print(name, r, g, b)   \
short name[3] = { r, g, b };                \
printf(#name);                              \
printf(": ");                               \
print_color(determin_color(name));          \
printf("\n");                               

int main() 
{
    calibrate_sensor();
    determin_and_print(redish,    204,   0,   0 );
    determin_and_print(greenish,    0, 153,  51 );
    determin_and_print(blueish,     0,  51, 204 );
    determin_and_print(yellowish, 204, 204,   0 );
    determin_and_print(pinkish,   255,  77, 255 );
    determin_and_print(cyanish,     0, 255, 153 );
    determin_and_print(brownish,  102,  51,   0 );
    determin_and_print(darkgray,   64,  64,  64 );
    determin_and_print(lightgray, 191, 191, 191 );
    return 0;
}