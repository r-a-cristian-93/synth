#include "WaveTables.h"
#include <math.h>

float sine_table[LUT_SIZE];
float sine_table_lfo[LUT_SIZE];

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

void waveforms_initialize()
{
    generate_sine_table();
    generate_sine_table_lfo();
}