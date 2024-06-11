#include "OrganOscillator.h"

#include <iostream>
#include <math.h>

#include "Config.h"
#include "Parameter.h"
#include "WaveTables.h"
#include "NoteFrequency.h"

float phase_increment[MIDI_NOTES_COUNT][DRAWBARS_COUNT] = {{0}};

Parameter drawbar_amplitude[DRAWBARS_COUNT];

void generate_phase_increment() {
    for (int midiNote = 0; midiNote < MIDI_NOTES_COUNT; midiNote++) {
        for (int drawbar_index = 0; drawbar_index < DRAWBARS_COUNT; drawbar_index++) {
            phase_increment[midiNote][drawbar_index] =
                note_frequency[midiNote][drawbar_index] * PHASE_LUT_SCALE_FACTOR ;
        }
    }
}

void organ_oscillator_init()
{
    generate_phase_increment();
    EnvelopeAdsr_Init();
}

void organ_oscillator_update()
{
    for (int drawbar_index = 0; drawbar_index < DRAWBARS_COUNT; drawbar_index++)
    {
        drawbar_amplitude[drawbar_index].update();
    }
}

double organ_oscillator_generate_sample(Note& note)
{
    double sample = 0;

    for (int drawbar_index = 0; drawbar_index < DRAWBARS_COUNT; drawbar_index++)
    {

        sample += sine_table[(int)(note.phaseAccumulator[drawbar_index] )]
            * drawbar_amplitude[drawbar_index].current_value * EnvelopeAdsr_GetAmplitude(&note.envelope)
            * HEADROOM_SCALE_FACTOR;

        note.phaseAccumulator[drawbar_index] += phase_increment[note.midiNote][drawbar_index];

        if (note.phaseAccumulator[drawbar_index]  >= LUT_SIZE)
            note.phaseAccumulator[drawbar_index]  -= LUT_SIZE;

        if (note.phaseAccumulator[drawbar_index]  < 0)
            note.phaseAccumulator[drawbar_index]  += LUT_SIZE;
    }

    return sample;
}

void organ_oscillator_set_drawbar_amplitude(int drawbar_index, float amplitude) {
    drawbar_amplitude[drawbar_index].target_value = amplitude;
}
