#ifndef ORGAN_OSCILATOR_H
#define ORGAN_OSCILATOR_H

#include <OrganEngine/Note.h>
#include <OrganEngine/Parameter.h>
#include <OrganEngine/WaveTables.h>
#include <iostream>

extern float notePhaseIncrement[MIDI_NOTES_COUNT][DRAWBARS_COUNT];
extern Parameter drawbarAmplitude[DRAWBARS_COUNT];

void organ_oscillator_initialize();
void organ_oscillator_set_drawbar_amplitude(uint8_t drawbar, float amplitude);

// Inline
int32_t organ_oscillator_generate_sample(Note& note);
void organ_oscillator_update();


 __attribute__((always_inline)) inline
void organ_oscillator_update()
{
    for (int drawbar_index = 0; drawbar_index < DRAWBARS_COUNT; drawbar_index++)
    {
        drawbarAmplitude[drawbar_index].update();
    }
}

 __attribute__((always_inline)) inline
int32_t organ_oscillator_generate_sample(Note& note)
{
    int32_t sample = 0;

    for (int drawbar_index = 0; drawbar_index < DRAWBARS_COUNT; drawbar_index++)
    {
        sample += sine_table[(int)(note.phaseAccumulator[drawbar_index] )];
            //* drawbarAmplitude[drawbar_index].current_value * note.envelope.getAmplitude()
            //* HEADROOM_SCALE_FACTOR;

        note.phaseAccumulator[drawbar_index] += notePhaseIncrement[note.midiNote][drawbar_index];

        if (note.phaseAccumulator[drawbar_index]  >= LUT_SIZE)
            note.phaseAccumulator[drawbar_index]  -= LUT_SIZE;

        if (note.phaseAccumulator[drawbar_index]  < 0)
            note.phaseAccumulator[drawbar_index]  += LUT_SIZE;
    }

    return sample;
}

#endif
