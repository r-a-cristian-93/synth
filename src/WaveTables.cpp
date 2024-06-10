#include "WaveTables.h"
#include <math.h>

float sine_table[LUT_SIZE];
float sine_table_lfo[LUT_SIZE];
float triangle_table[LUT_SIZE];
float smooth_triangle_table[LUT_SIZE];

void generate_sine_table()
{
    for (int i = 0; i < LUT_SIZE; i++)
    {
        sine_table[i] = sin(M_2PI * i / LUT_SIZE);
    }
}

void generate_sine_table_lfo()
{
    for (int i = 0; i < LUT_SIZE; i++)
    {
        sine_table_lfo[i] = (sine_table[i] +1.0) * 0.5;
    }
}

void generate_triangle_table()
{
    float alpha = 0.1;

    for (int i = 0; i < LUT_SIZE; ++i)
    {
        triangle_table[i] = asin(cos(M_2PI * i / LUT_SIZE)) / 1.571;
    }
}

void generate_smooth_triangle_table()
{
    for (int i = 0; i < LUT_SIZE; ++i)
    {
        triangle_table[i] = asin(cos(M_2PI * i / LUT_SIZE));
    }
}

void waveforms_initialize()
{
    generate_sine_table();
    generate_sine_table_lfo();
    generate_triangle_table();
}