#ifndef ORGAN_OSCILATOR_H
#define ORGAN_OSCILATOR_H

#include "Note.h"

extern float note_frequency[MIDI_NOTES_COUNT][DRAWBARS_COUNT];

void osc_init();
void osc_update();
double osc_generate_sample(Note& note);

void osc_set_drawbar_amplitude(int drawbar, float amplitude);
void osc_set_vibrato_fast();
void osc_set_vibrato_slow();
void osc_set_vibrato_off();

#endif
