#include <WaveOrgan/WaveOrgan.h>

LPF lpf = {};
SoftClip softClip = {};

float wav_phase[MIDI_NOTES_COUNT] = {0};
float wav_phaseIncrement[MIDI_NOTES_COUNT] = {0};
float wav_frequency[MIDI_NOTES_COUNT] = {0};
Envelope wav_notes[MIDI_NOTES_COUNT] = {};
uint8_t wav_bass_volume = 127;
uint8_t wav_orchestra_volume = 127;


int16_t voice_lut[32][LUT_SIZE] = {0};
int16_t effect_lut[32][LUT_SIZE] = {0};

int16_t* wav_active_voice = voice_lut[0];
int16_t* wav_active_effect = effect_lut[0];

void fill_voice_combinations()
{
    // First voice is silence, skip it
    for (int mask = 1; mask < 32; ++mask)
    {
        uint8_t voice_on_flute8 = mask & (1 << 0);
        uint8_t voice_on_flute4 = (mask & (1 << 1)) >> 1;
        uint8_t voice_on_clarinet = (mask & (1 << 2)) >> 2;
        uint8_t voice_on_trumpet = (mask & (1 << 3)) >> 3;
        uint8_t voice_on_violin = (mask & (1 << 4)) >> 4;

        for (uint16_t lutIndex = 0; lutIndex < LUT_SIZE; lutIndex++)
        {
            int32_t sample = 0;

            sample += wav_flute8[lutIndex] * voice_on_flute8;
            sample += wav_flute4[lutIndex] * voice_on_flute4;
            sample += wav_clarinet[lutIndex] * voice_on_clarinet;
            sample += wav_trumpet[lutIndex] * voice_on_trumpet;
            sample += wav_violin[lutIndex] * voice_on_violin;

            // Scale based on number of voices;
            sample = sample / (
                voice_on_flute8 +
                voice_on_flute4 +
                voice_on_clarinet +
                voice_on_trumpet +
                voice_on_violin +
                1
            );

            voice_lut[mask][lutIndex] = sample;
        }
    }
}


void fill_effect_combinations()
{
    // First voice is silence, skip it
    for (int mask = 1; mask < 32; ++mask)
    {
        uint8_t effect_on_perc4 = mask & (1 << 0);
        uint8_t effect_on_piano = (mask & (1 << 1)) >> 1;
        uint8_t effect_on_harpsi = (mask & (1 << 2)) >> 2;
        uint8_t effect_on_bells = (mask & (1 << 3)) >> 3;
        uint8_t effect_on_synth = (mask & (1 << 4)) >> 4;

        for (uint16_t lutIndex = 0; lutIndex < LUT_SIZE; lutIndex++)
        {
            int32_t sample = 0;

            sample += wav_perc4[lutIndex] *   effect_on_perc4;
            sample += wav_piano[lutIndex] *   effect_on_piano;
            sample += wav_harpsi[lutIndex] * effect_on_harpsi;
            sample += wav_bells[lutIndex] *  effect_on_bells;
            sample += wav_synth[lutIndex] *   effect_on_synth;

            // Scale based on number of voices;
            sample = sample / (
                effect_on_perc4 +
                effect_on_piano +
                effect_on_harpsi +
                effect_on_bells +
                effect_on_synth +
                1
            );

            effect_lut[mask][lutIndex] = sample;
        }
    }
}

void wave_organ_set_voice(uint8_t voiceIndex)
{
	if (voiceIndex < 32)
		wav_active_voice = voice_lut[voiceIndex];
}

void wave_organ_set_effect(uint8_t effectIndex)
{
	if (effectIndex < 32)
		wav_active_effect = effect_lut[effectIndex];
}


