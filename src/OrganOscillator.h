#ifndef ORGAN_OSCILATOR_H
#define ORGAN_OSCILATOR_H

#include "Note.h"

extern float note_frequency[MIDI_NOTES_COUNT][DRAWBARS_COUNT];

void osc_init();
void osc_update();
double osc_generate_sample(Note& note);

void osc_set_drawbar_amplitude(int drawbar, float amplitude);


#endif
