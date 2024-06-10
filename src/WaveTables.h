#ifndef WAVEFORMS_H
#define WAVEFORMS_H

#include "Config.h"

extern float sine_table[LUT_SIZE];
extern float sine_table_lfo[LUT_SIZE];
extern float smooth_triangle_table[LUT_SIZE];
extern float smooth_square_table[LUT_SIZE];
extern float smooth_sawtooth_table[LUT_SIZE];

void waveforms_initialize();

#endif