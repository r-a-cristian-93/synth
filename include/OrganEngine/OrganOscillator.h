#ifndef ORGAN_OSCILATOR_H
#define ORGAN_OSCILATOR_H

#include <OrganEngine/NoteManager.h>
#include <OrganEngine/Parameter.h>
#include <OrganEngine/WaveTables.h>
#include <iostream>

extern float notePhaseIncrement[MIDI_NOTES_COUNT][DRAWBARS_COUNT];
extern Parameter drawbarAmplitude[DRAWBARS_COUNT];

extern float tonewheelPhaseIncrement[TONEWHEELS];
extern float tonewheelPhase[TONEWHEELS];
extern uint16_t tonewheelAmplitude[TONEWHEELS];
extern uint8_t tonewheelMap[MANUAL_KEYS][DRAWBARS_COUNT];
extern int16_t* tonewheelWaveform;

void organ_oscillator_initialize();
void organ_oscillator_set_drawbar_amplitude(uint8_t controller, uint8_t midiValue);

void drawbar_amplitude_update();


void map_keys_to_tonewheel();
int foldback(uint8_t tonewheel);
int getTonewheelIndex(int key, int drawbar);

// Inline
void reset_tonewheel_amplitude();
void set_tonewheels_amplitude();
int16_t organ_oscillator_generate_sample();




 __attribute__((always_inline)) inline
int16_t organ_oscillator_generate_sample()
{
    drawbar_amplitude_update();

    int32_t sample = 0;

    for (int tonewheelIndex = 0; tonewheelIndex < TONEWHEELS; tonewheelIndex++)
    {
        sample += ((tonewheelWaveform[(int)(tonewheelPhase[tonewheelIndex])] * tonewheelAmplitude[tonewheelIndex]) >> 16);

        tonewheelPhase[tonewheelIndex] += tonewheelPhaseIncrement[tonewheelIndex];

        while (tonewheelPhase[tonewheelIndex]  >= LUT_SIZE)
            tonewheelPhase[tonewheelIndex]  -= LUT_SIZE;

        while (tonewheelPhase[tonewheelIndex]  < 0)
            tonewheelPhase[tonewheelIndex]  += LUT_SIZE;
    }
    return (int16_t) sample;
}

 __attribute__((always_inline)) inline
void set_next_waveform() {
    if (tonewheelWaveform == sine_table)
    {
        tonewheelWaveform = smooth_triangle_table;
    }
    else if (tonewheelWaveform == smooth_triangle_table)
    {
        tonewheelWaveform = smooth_square_table;
    }
    else if (tonewheelWaveform == smooth_square_table)
    {
        tonewheelWaveform = smooth_sawtooth_table;
    }
    else if (tonewheelWaveform == smooth_sawtooth_table)
    {
        tonewheelWaveform = sine_table;
    }
}

#endif
