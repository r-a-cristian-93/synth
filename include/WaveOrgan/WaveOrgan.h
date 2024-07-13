#ifndef WAVE_ORGAN_H
#define WAVE_ORGAN_H

#ifndef MIDI_NOTES_COUNT
#define MIDI_NOTES_COUNT (128)
#endif

#ifndef LUT_SIZE
#define LUT_SIZE (1024)
#endif

#include <cstdint>

constexpr uint8_t VOICES_COUNT = 5;

#include <cstdint>
#include <math.h>
#include <WaveOrgan/flute4.h>
#include <WaveOrgan/flute8.h>
#include <WaveOrgan/clarinet.h>
#include <WaveOrgan/trumpet.h>
#include <WaveOrgan/violin.h>
#include <WaveOrgan/bass.h>

extern float wav_phase[MIDI_NOTES_COUNT];
extern float wav_phaseIncrement[MIDI_NOTES_COUNT];
extern float wav_frequency[MIDI_NOTES_COUNT];
extern bool wav_notes[MIDI_NOTES_COUNT];
extern int16_t voice_lut[32][LUT_SIZE];
extern int16_t* wav_active_voice;
extern uint8_t wav_bass_volume;

void wave_organ_set_voice(uint8_t voiceIndex);
void fill_voice_combinations();


__attribute__((always_inline)) inline int32_t wave_organ_generate_sample()
{
	int32_t sample = 0;

	for (uint16_t noteIndex = 0; noteIndex < MIDI_NOTES_COUNT; noteIndex++)
	{
		if (wav_notes[noteIndex] == false)
			continue;

        sample += wav_active_voice[(uint16_t) wav_phase[noteIndex]];
        sample += (wav_bass[(uint16_t) wav_phase[noteIndex]] * wav_bass_volume);

		wav_phase[noteIndex] += wav_phaseIncrement[noteIndex];

        while (wav_phase[noteIndex] >= LUT_SIZE)
        {
            wav_phase[noteIndex] -= LUT_SIZE;
        }

        while (wav_phase[noteIndex] < 0)
        {
            wav_phase[noteIndex] += LUT_SIZE;
        }
    }

	return sample >> 4;
}

__attribute__((always_inline)) inline
void wave_organ_init()
{
	for (int i = 0; i < MIDI_NOTES_COUNT; i++)
	{
		wav_frequency[i] = 110.0 * pow(2.0, ((i - 21) / 12.0));
        wav_phaseIncrement[i] = wav_frequency[i]  * ((float)LUT_SIZE / 44100.0) ;
	}

    fill_voice_combinations();
    wave_organ_set_voice(1);
}

__attribute__((always_inline)) inline
void wav_organ_note_on(uint8_t midiNote) {
    wav_notes[midiNote -36] = true;
}

__attribute__((always_inline)) inline
void wav_organ_note_off(uint8_t midiNote) {
    wav_notes[midiNote-36] = false;
}

__attribute__((always_inline)) inline
void wave_organ_set_bass_volume(uint8_t volume) {
    wav_bass_volume = volume;
}



#endif