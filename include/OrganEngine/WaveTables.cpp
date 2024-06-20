#include "WaveTables.h"
#include <math.h>

float sine_table[LUT_SIZE];
float sine_table_lfo[LUT_SIZE];
float smooth_triangle_table[LUT_SIZE];
float smooth_square_table[LUT_SIZE];
float smooth_sawtooth_table[LUT_SIZE];

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

void generate_smooth_triangle_table()
{
    for (int i = 0; i < LUT_SIZE; ++i)
    {
        smooth_triangle_table[i] = sin(M_2PI * i / LUT_SIZE) + pow(sin(M_2PI * i / LUT_SIZE), 3) * 0.5;
    }
}

void generate_smooth_square_table()
{
    for (int i = 0; i < LUT_SIZE; ++i)
    {
        smooth_square_table[i] = tanh(sin(M_2PI * i / LUT_SIZE) * 4.0);
    }
}

void generate_smooth_sawtooth_table()
{
    for (int i = 0; i < LUT_SIZE; ++i)
    {
        float phase = M_2PI * i / LUT_SIZE;
        smooth_sawtooth_table[i] = asin(tanh(5*cos(phase))*sin(phase));
    }
}

void waveforms_initialize()
{
    generate_sine_table();
    generate_sine_table_lfo();
    generate_smooth_triangle_table();
    generate_smooth_square_table();
    generate_smooth_sawtooth_table();
}