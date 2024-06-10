#include "Waveforms.h"
#include <math.h>

float sine_table[LUT_SIZE];

void generate_sine_table() {
    for (int i = 0; i < LUT_SIZE; i++) {
        sine_table[i] = sin(M_2PI * i / LUT_SIZE);
    }
}

void waveforms_initialize() {
    generate_sine_table();
}