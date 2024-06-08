#ifndef ORGAN_SYNTHESIZER_H
#define ORGAN_SYNTHESIZER_H

#include <vector>
#include <iostream>

#include "OrganOscillator.h"


int midi_init();
void midi_close();

int audio_driver_init();
void audio_driver_close();


#endif