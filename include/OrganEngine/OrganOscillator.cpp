#include <OrganEngine/OrganOscillator.h>
#include <OrganEngine/Config.h>
#include <OrganEngine/NoteFrequency.h>

float notePhaseIncrement[MIDI_NOTES_COUNT][DRAWBARS_COUNT] = {{0}};
Parameter drawbarAmplitude[DRAWBARS_COUNT];

void generate_phase_increment() {
    for (int midiNote = 0; midiNote < MIDI_NOTES_COUNT; midiNote++) {
        for (int drawbarIndex = 0; drawbarIndex < DRAWBARS_COUNT; drawbarIndex++) {
            notePhaseIncrement[midiNote][drawbarIndex] =
                noteFrequency[midiNote][drawbarIndex] * PHASE_LUT_SCALE_FACTOR ;
        }
    }
}

void organ_oscillator_initialize()
{
    generate_phase_increment();
}

void organ_oscillator_set_drawbar_amplitude(int drawbar_index, float amplitude) {
    drawbarAmplitude[drawbar_index].target_value = amplitude;
}
