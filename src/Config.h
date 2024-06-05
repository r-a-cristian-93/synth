#ifndef CONFIG_H
#define CONFIG_H

#include <cstdint>

#define SAMPLE_RATE 44100.0
#define LUT_SIZE 1024

#define M_2PI 6.28318530717958647692
#define TWO_PI_OVER_SAMPLE_RATE  (M_2PI / SAMPLE_RATE)
#define PARAM_LOWEST_VALUE 0.0001
#define PHASE_LUT_SCALE_FACTOR (LUT_SIZE / M_2PI)
#define DRAWBARS_COUNT 9
#define MIDI_NOTES_COUNT 128

extern double g_time;
extern double g_amplitude;

#endif