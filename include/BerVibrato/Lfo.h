#ifndef LFO_H_
#define LFO_H_

#define _USE_MATH_DEFINES
#include <math.h>

struct Lfo
{
	float sampleRate;
	float frequency;
	float phase;
	float index;
	float phaseIncrement;
};

void lfo_initialize(float sampleRate, float freq);
void lfo_set_frequency(float freq);
void lfo_update();
float lfo_get_value();


#endif




// Parameter vibrato_phase_increment
// {
//     VIBRATO_FAST,
//     VIBRATO_FAST,
//     0.001,
//     VIBRATO_FAST,
//     0.0,
//     0.01 / SAMPLE_RATE
// };


//     vibrato_phase_increment.update();


// float vibrato_phase_accumulator;

// void organ_oscillator_set_vibrato_fast() {
//     vibrato_phase_increment.setValue(VIBRATO_FAST);
// }

// void organ_oscillator_set_vibrato_slow() {
//     vibrato_phase_increment.setValue(VIBRATO_SLOW);
// }

// void organ_oscillator_set_vibrato_off() {
//     vibrato_phase_increment.setValue(0.0);
// }
