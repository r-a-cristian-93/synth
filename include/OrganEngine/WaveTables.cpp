#include "WaveTables.h"
#include <math.h>
#include <iostream>

int16_t sine_table[LUT_SIZE];
int16_t sine_table_lfo[LUT_SIZE];


void generate_sine_table()
{
    for (int i = 0; i < LUT_SIZE; i++)
    {
        sine_table[i] = sin(M_2PI * i / LUT_SIZE) * MAX_AMPLITUDE;
    }
}

void generate_sine_table_lfo()
{
    for (int i = 0; i < LUT_SIZE; i++)
    {
        sine_table_lfo[i] = (sine_table[i] + MAX_AMPLITUDE) * 0.5;
    }
}

void waveforms_initialize()
{
    generate_sine_table();
    generate_sine_table_lfo();
}