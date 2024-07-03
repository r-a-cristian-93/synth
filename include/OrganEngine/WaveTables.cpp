#include "WaveTables.h"
#include <math.h>
#include <iostream>

int16_t sine_table[LUT_SIZE];
int16_t sine_table_lfo[LUT_SIZE];
int16_t smooth_triangle_table[LUT_SIZE];
int16_t smooth_square_table[LUT_SIZE];
int16_t smooth_sawtooth_table[LUT_SIZE];


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

double smoothTriangleFourier(double x, int terms = 5) {
    double result = 0.0;
    for (int k = 0; k < terms; ++k) {
        int n = 2 * k + 1;
        result += pow(-1, k) * (sin(n * x) / (n * n));
    }
    return (8 / (M_PI * M_PI)) * result;
}

void generate_smooth_triangle_table()
{
    for (int i = 0; i < LUT_SIZE; ++i)
    {
        smooth_triangle_table[i] = smoothTriangleFourier(M_2PI * i / LUT_SIZE) * MAX_AMPLITUDE;
    }
}

void generate_smooth_square_table()
{
    for (int i = 0; i < LUT_SIZE; ++i)
    {
        smooth_square_table[i] = tanh(sin(M_2PI * i / LUT_SIZE) * 4.0) * MAX_AMPLITUDE;
    }
}


// s        m
// ------------
// 1	    2.2
// 2	    1.35
// 3	    1.1
// 4	    0.98
// 5	    0.92
// 10	    0.78
// 20	    0.71
// 50	    0.69
// 100	    0.65
// 200	    0.64
// 500	    0.64
// 1000	0.64

void generate_smooth_sawtooth_table()
{
    float s = 20;
    float m = 0.71;

    for (int i = 0; i < LUT_SIZE; ++i)
    {
        float phase = M_2PI * i / LUT_SIZE * 0.5;
        smooth_sawtooth_table[i] = asin(tanh(s*cos(phase))*sin(phase)) * m * MAX_AMPLITUDE;
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