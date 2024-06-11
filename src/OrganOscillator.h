#ifndef ORGAN_OSCILATOR_H
#define ORGAN_OSCILATOR_H

#include "Note.h"

void organ_oscillator_init();
void organ_oscillator_update();
double organ_oscillator_generate_sample(Note& note);

void organ_oscillator_set_drawbar_amplitude(int drawbar, float amplitude);

#endif
