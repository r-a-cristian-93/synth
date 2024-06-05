#ifndef COMMON_H
#define COMMON_H

#include "SystemConfig.h"

#define M_2PI 6.28318530717958647692
#define TWO_PI_OVER_SAMPLE_RATE  (M_2PI / SAMPLE_RATE)
#define DRAWBARS_COUNT 9
#define MIDI_NOTES_COUNT 128
#define PARAM_LOWEST_VALUE 0.0001

extern double g_time;
extern double g_amplitude;

#endif
