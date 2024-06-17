#ifndef WAVEFORMS_H
#define WAVEFORMS_H

#include <OrganEngine/Config.h>

extern float sine_table[LUT_SIZE];
extern float sine_table_lfo[LUT_SIZE];

void waveforms_initialize();

#endif