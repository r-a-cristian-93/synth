#ifndef WAVEFORMS_H
#define WAVEFORMS_H

#include <OrganEngine/Config.h>

extern uint16_t sine_table[LUT_SIZE];
extern uint16_t sine_table_lfo[LUT_SIZE];

void waveforms_initialize();

#endif