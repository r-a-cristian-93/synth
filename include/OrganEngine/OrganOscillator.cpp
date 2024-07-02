#include <OrganEngine/OrganOscillator.h>
#include <OrganEngine/Config.h>
#include <OrganEngine/NoteFrequency.h>
#include <OrganEngine/TonewheelsFrequency.h>
#include <OrganEngine/MidiManager.h>

float notePhaseIncrement[MIDI_NOTES_COUNT][DRAWBARS_COUNT] = {{0}};
Parameter drawbarAmplitude[DRAWBARS_COUNT];

float tonewheelPhaseIncrement[TONEWHEELS] = {0};
float tonewheelPhase[TONEWHEELS] = {0};
uint16_t tonewheelAmplitude[TONEWHEELS] = {0};
uint8_t tonewheelMap[MANUAL_KEYS][DRAWBARS_COUNT] = {{0}};

void drawbar_amplitude_update()
{
    for (uint8_t drawbarIndex = 0; drawbarIndex < DRAWBARS_COUNT; drawbarIndex++)

    drawbarAmplitude[drawbarIndex].update();
}

//  __attribute__((always_inline)) inline
void reset_tonewheel_amplitude() {
    for (uint8_t tonewheelIndex = 0; tonewheelIndex < TONEWHEELS; tonewheelIndex++)
    {
        tonewheelAmplitude[tonewheelIndex] = 0;
    }
}

//  __attribute__((always_inline)) inline
void set_tonewheels_amplitude()
{
    for (uint8_t drawbarIndex = 0; drawbarIndex < DRAWBARS_COUNT; drawbarIndex++)
    {
        int16_t dAmplitude = drawbarAmplitude[drawbarIndex].current_value;

        for (uint8_t keyIndex = 0; keyIndex < MANUAL_KEYS; keyIndex++)
        {
            if (keysList[keyIndex] == KEY_STATE_ON)
            {
                uint8_t tonewheelIndex = tonewheelMap[keyIndex][drawbarIndex];

                tonewheelAmplitude[tonewheelIndex] += (dAmplitude >> 5);
            }
        }
    }
}

void generate_phase_increment() {
    for (int midiNote = 0; midiNote < MIDI_NOTES_COUNT; midiNote++) {
        for (int drawbarIndex = 0; drawbarIndex < DRAWBARS_COUNT; drawbarIndex++) {
            notePhaseIncrement[midiNote][drawbarIndex] =
                noteFrequency[midiNote][drawbarIndex] * PHASE_LUT_SCALE_FACTOR ;
        }
    }

    for (int tonewheelIndex = 0; tonewheelIndex < TONEWHEELS; tonewheelIndex++) {
        tonewheelPhaseIncrement[tonewheelIndex] = tonewheelFrequency[tonewheelIndex] * PHASE_LUT_SCALE_FACTOR;
    }
}

void organ_oscillator_initialize()
{
    generate_phase_increment();
    map_keys_to_tonewheel();
}

void organ_oscillator_set_drawbar_amplitude(uint8_t controller, uint8_t midiValue) {
    drawbarAmplitude[get_drawbar_id(controller)].target_value = midiValue * INT8_TO_INT16_SCALE_FACTOR;
}


void map_keys_to_tonewheel() {
    for (uint8_t keyIndex = 0; keyIndex < MANUAL_KEYS; keyIndex++)
    {
        for (uint8_t drawbarIndex = 0; drawbarIndex < 9; drawbarIndex++)
        {
            tonewheelMap[keyIndex][drawbarIndex] = getTonewheelIndex(keyIndex, drawbarIndex);

            std::cout << (int) tonewheelMap[keyIndex][drawbarIndex] << ", ";
        }

        std::cout << std::endl;
    }
}

int foldback(uint8_t tonewheel) {
    while (tonewheel < 12) {
        tonewheel += 12;
    }
    while (tonewheel > 91) {
        tonewheel -= 12;
    }
    return tonewheel;
}

int getTonewheelIndex(int key, int drawbar) {
    switch (drawbar) {
    case 0: // Sub-octave; 16'
        return foldback(key);
    case 1: // 5th; 5 1/3'
        return foldback(key + 19);
    case 2: // Unison; 8'
        return foldback(key + 12);
    case 3: // 8th (Octave); 4'
        return foldback(key + 24);
    case 4: // 12th; 2 2/3'
        return foldback(key + 31);
    case 5: // 15th; 2'
        return foldback(key + 36);
    case 6: // 17th; 1 3/5'
        return foldback(key + 40);
    case 7: // 19th; 1 1/3'
        return foldback(key + 43);
    case 8: // 22nd; 1'
        return foldback(key + 48);
    }
    return 0;
}

