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
int16_t organ_oscillator_generate_sample(Note& note);
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
int16_t organ_oscillator_generate_sample(Note& note)
{
    int32_t sample = 0;

    for (int drawbar_index = 0; drawbar_index < DRAWBARS_COUNT; drawbar_index++)
    {
        // 16bit * 16bit = 32bit
        // right shift 16 to bring back in 16bit range
        sample += (sine_table[(int)(note.phaseAccumulator[drawbar_index] )] * note.envelope.getAmplitude()) >> 16;

        note.phaseAccumulator[drawbar_index] += notePhaseIncrement[note.midiNote][drawbar_index];

        if (note.phaseAccumulator[drawbar_index]  >= LUT_SIZE)
            note.phaseAccumulator[drawbar_index]  -= LUT_SIZE;

        if (note.phaseAccumulator[drawbar_index]  < 0)
            note.phaseAccumulator[drawbar_index]  += LUT_SIZE;
    }

    // Right shift 4 to take account of drawbars and headroom. Good enugh for  24 notes polyphony.
    return (int16_t) (sample >> 4);
}

#endif
