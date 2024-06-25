#include "WaveTables.h"
#include <math.h>
#include <iostream>

uint16_t sine_table[LUT_SIZE];
uint16_t sine_table_lfo[LUT_SIZE];


void generate_sine_table()
{
    for (int i = 0; i < LUT_SIZE; i++)
    {
        sine_table[i] = sin(M_2PI * i / LUT_SIZE) * MAX_AMPLITUDE + MAX_AMPLITUDE;
        std::cout << sine_table[i] <<  "  ";
    }
}

void generate_sine_table_lfo()
{
    for (int i = 0; i < LUT_SIZE; i++)
    {
        sine_table_lfo[i] = sine_table[i];
    }
}

void waveforms_initialize()
{
    generate_sine_table();
    generate_sine_table_lfo();
}