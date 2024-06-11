#include "OrganOscillator.h"

#include "Config.h"
#include "NoteFrequency.h"

float note_phase_increment[MIDI_NOTES_COUNT][DRAWBARS_COUNT] = {{0}};
Parameter drawbar_amplitude[DRAWBARS_COUNT];

void generate_phase_increment() {
    for (int midiNote = 0; midiNote < MIDI_NOTES_COUNT; midiNote++) {
        for (int drawbar_index = 0; drawbar_index < DRAWBARS_COUNT; drawbar_index++) {
            note_phase_increment[midiNote][drawbar_index] =
                note_frequency[midiNote][drawbar_index] * PHASE_LUT_SCALE_FACTOR ;
        }
    }
}

void organ_oscillator_init()
{
    generate_phase_increment();
    EnvelopeAdsr_Init();
}

void organ_oscillator_set_drawbar_amplitude(int drawbar_index, float amplitude) {
    drawbar_amplitude[drawbar_index].target_value = amplitude;
}
