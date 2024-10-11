#ifndef WAVE_ORGAN_H
#define WAVE_ORGAN_H

#ifndef MIDI_NOTES_COUNT
#define MIDI_NOTES_COUNT (128)
#endif

#ifndef LUT_SIZE
#define LUT_SIZE (1024)
#endif

#include <cstdint>
#include <math.h>
#include <WaveOrgan/orchestra/flute4.h>
#include <WaveOrgan/orchestra/flute8.h>
#include <WaveOrgan/orchestra/clarinet.h>
#include <WaveOrgan/orchestra/trumpet.h>
#include <WaveOrgan/orchestra/violin.h>
#include <WaveOrgan/orchestra/bass.h>
#include <WaveOrgan/Envelope.h>

#include <WaveOrgan/effects/perc4.h>
#include <WaveOrgan/effects/piano.h>
#include <WaveOrgan/effects/harpsi.h>
#include <WaveOrgan/effects/bells.h>
#include <WaveOrgan/effects/synth.h>
#include <WaveOrgan/SoftClip.h>

struct LPF {
    int32_t output = 0.0;
    uint8_t alpha = 110;

    int32_t getSample(int32_t input) {

        output = ((127 - alpha) * input + alpha * output) >> 7;

        return output;
    }
};

extern LPF lpf;
extern SoftClip softClip;

extern float wav_phase[MIDI_NOTES_COUNT];
extern float wav_phaseIncrement[MIDI_NOTES_COUNT];
extern float wav_frequency[MIDI_NOTES_COUNT];
extern Envelope wav_notes[MIDI_NOTES_COUNT];

extern int16_t voice_lut[32][LUT_SIZE];
extern int16_t effect_lut[32][LUT_SIZE];
extern int16_t* wav_active_voice;
extern int16_t* wav_active_effect;
extern uint8_t wav_orchestra_volume;
extern uint8_t wav_bass_volume;

void wave_organ_set_voice(uint8_t voiceIndex);
void wave_organ_set_effect(uint8_t voiceIndex);
void fill_voice_combinations();
void fill_effect_combinations();


__attribute__((always_inline)) inline
int32_t wave_organ_generate_sample()
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
		float envelopeAmpl = envelope_get_amplitude(&wav_notes[noteIndex]);
        sample += (int32_t)((float)wav_active_effect[(uint16_t) wav_phase[noteIndex]] * envelopeAmpl * wav_orchestra_volume);

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
    sample = sample >> 8;
    sample = SoftClip_ProcessSample(&softClip, sample);

	return sample;
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
    fill_effect_combinations();
    wave_organ_set_voice(1);
    wave_organ_set_effect(1);

    SoftClip_Init(&softClip, 65536, 2047, 2.0);
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
    lpf.alpha = alpha;
}



#endif