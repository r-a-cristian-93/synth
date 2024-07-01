#ifndef ORGAN_OSCILATOR_H
#define ORGAN_OSCILATOR_H

#include <OrganEngine/Note.h>
#include <OrganEngine/Parameter.h>
#include <OrganEngine/WaveTables.h>
#include <iostream>

extern float notePhaseIncrement[MIDI_NOTES_COUNT][DRAWBARS_COUNT];
extern Parameter drawbarAmplitude[DRAWBARS_COUNT];

extern float tonewheelPhaseIncrement[TONEWHEELS];
extern float tonewheelPhase[TONEWHEELS];
extern uint16_t tonewheelAmplitude[TONEWHEELS];
extern uint8_t tonewheelMap[61][9];

void organ_oscillator_initialize();
void organ_oscillator_set_drawbar_amplitude(uint8_t drawbar, float amplitude);

void map_keys_to_tonewheel();
int foldback(uint8_t tonewheel);
int getTonewheelIndex(int key, int drawbar);

// Inline
int16_t organ_oscillator_generate_sample(Note& note);
void organ_oscillator_update();
void organ_oscillator_increment_phase();


 __attribute__((always_inline)) inline
void organ_oscillator_update()
{
    // for (int drawbar_index = 0; drawbar_index < DRAWBARS_COUNT; drawbar_index++)
    // {
    //     drawbarAmplitude[drawbar_index].update();
    // }

    // organ_oscillator_increment_phase();
}

 __attribute__((always_inline)) inline
void organ_oscillator_increment_phase()
{
    // for (uint8_t tonewheelIndex = 0; tonewheelIndex < TONEWHEELS; tonewheelIndex++)
    // {
    //     tonewheelPhase[tonewheelIndex] += tonewheelPhaseIncrement[tonewheelIndex];

    //     while (tonewheelPhase[tonewheelIndex]  >= LUT_SIZE)
    //         tonewheelPhase[tonewheelIndex]  -= LUT_SIZE;

    //     while (tonewheelPhase[tonewheelIndex]  < 0)
    //         tonewheelPhase[tonewheelIndex]  += LUT_SIZE;
    // }
}

 __attribute__((always_inline)) inline
int16_t organ_oscillator_generate_sample()
{
    int32_t sample = 0;

    for (int tonewheelIndex = 0; tonewheelIndex < TONEWHEELS; tonewheelIndex++)
    {
        sample += ((sine_table[(int)(tonewheelPhase[tonewheelIndex])] * tonewheelAmplitude[tonewheelIndex]) >> 16);
        tonewheelPhase[tonewheelIndex] += tonewheelPhaseIncrement[tonewheelIndex];

        while (tonewheelPhase[tonewheelIndex]  >= LUT_SIZE)
            tonewheelPhase[tonewheelIndex]  -= LUT_SIZE;

        while (tonewheelPhase[tonewheelIndex]  < 0)
            tonewheelPhase[tonewheelIndex]  += LUT_SIZE;
    }
    return (int16_t) sample;
}

#endif
