#ifndef ORGAN_OSCILATOR_H
#define ORGAN_OSCILATOR_H

#include "Note.h"
#include "Parameter.h"
#include "WaveTables.h"

extern float note_phase_increment[MIDI_NOTES_COUNT][DRAWBARS_COUNT];
extern Parameter drawbar_amplitude[DRAWBARS_COUNT];

void organ_oscillator_init();
void organ_oscillator_set_drawbar_amplitude(int drawbar, float amplitude);

 __attribute__((always_inline)) inline
void organ_oscillator_update()
{
    for (int drawbar_index = 0; drawbar_index < DRAWBARS_COUNT; drawbar_index++)
    {
        drawbar_amplitude[drawbar_index].update();
    }
}

 __attribute__((always_inline)) inline
double organ_oscillator_generate_sample(Note& note)
{
    double sample = 0;

    for (int drawbar_index = 0; drawbar_index < DRAWBARS_COUNT; drawbar_index++)
    {
        sample += sine_table[(int)(note.phaseAccumulator[drawbar_index] )]
            * drawbar_amplitude[drawbar_index].current_value * EnvelopeAdsr_GetAmplitude(&note.envelope)
            * HEADROOM_SCALE_FACTOR;

        note.phaseAccumulator[drawbar_index] += note_phase_increment[note.midiNote][drawbar_index];

        if (note.phaseAccumulator[drawbar_index]  >= LUT_SIZE)
            note.phaseAccumulator[drawbar_index]  -= LUT_SIZE;

        if (note.phaseAccumulator[drawbar_index]  < 0)
            note.phaseAccumulator[drawbar_index]  += LUT_SIZE;
    }

    return sample;
}

#endif
