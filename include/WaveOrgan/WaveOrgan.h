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
#include <WaveOrgan/Envelope.h>

struct LPF {
    float output = 0.0;
    float alpha = 0.85;

    int32_t getSample(int32_t input) {

        output = (1.0 - alpha) * input + alpha * output;

        return (int32_t) output;
    }
};

extern LPF lpf;

extern float wav_phase[MIDI_NOTES_COUNT];
extern float wav_phaseIncrement[MIDI_NOTES_COUNT];
extern float wav_frequency[MIDI_NOTES_COUNT];
extern Envelope wav_notes[MIDI_NOTES_COUNT];

extern int16_t voice_lut[32][LUT_SIZE];
extern int16_t* wav_active_voice;
extern uint8_t wav_orchestra_volume;
extern uint8_t wav_bass_volume;

void wave_organ_set_voice(uint8_t voiceIndex);
void fill_voice_combinations();

__attribute__((always_inline)) inline int32_t wave_organ_generate_sample()
{
	int32_t sample = 0;

	for (uint16_t noteIndex = 0; noteIndex < MIDI_NOTES_COUNT; noteIndex++)
	{
		if (wav_notes[noteIndex].state == ADSR_IDLE)
			continue;

        if (wav_notes[noteIndex].state != ADSR_RELEASE) {
            // organ
            sample += (wav_active_voice[(uint16_t) wav_phase[noteIndex]] * wav_orchestra_volume);

            // bass
            sample += (wav_bass[(uint16_t) wav_phase[noteIndex]] * wav_bass_volume);
        }

        // piano
		float envelopeAmpl = envelope_get_amplitude(&wav_notes[noteIndex]) * 127;
        sample += (int32_t)((float)wav_active_voice[(uint16_t) wav_phase[noteIndex]] * envelopeAmpl);

        // lpf
        sample = lpf.getSample(sample);

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

    //   voice * vol * inst * keys
    // ( 32767 * 127 *  3   *  20 ) >> 13 = 30479
	return sample >> 13;
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
    wav_notes[midiNote - 36].state = ADSR_ATTACK;
}

__attribute__((always_inline)) inline
void wav_organ_note_off(uint8_t midiNote) {
    wav_notes[midiNote - 36].state = ADSR_RELEASE;
}

__attribute__((always_inline)) inline
void wave_organ_set_orchestra_volume(uint8_t volume) {
    wav_orchestra_volume = volume;
}

__attribute__((always_inline)) inline
void wave_organ_set_bass_volume(uint8_t volume) {
    wav_bass_volume = volume;
}

__attribute__((always_inline)) inline
void wave_organ_set_lpf(uint8_t alpha) {
    lpf.alpha = (float)alpha * (1.0 / 127.0);
}



#endif