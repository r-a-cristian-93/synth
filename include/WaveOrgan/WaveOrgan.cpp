#include <WaveOrgan/WaveOrgan.h>

float wav_phase[MIDI_NOTES_COUNT] = {0};
float wav_phaseIncrement[MIDI_NOTES_COUNT] = {0};
float wav_frequency[MIDI_NOTES_COUNT] = {0};
bool wav_notes[MIDI_NOTES_COUNT] = {false};
int32_t wav_lut[LUT_SIZE] = {0};
uint8_t wav_voices_on = 0;

const int16_t* wave_organ_get_voice_lut(uint8_t voiceIndex)
{
    switch (voiceIndex) {
        case 1:
            return wav_flute4;
        break;
        case 2:
            return wav_clarinet;
        break;
        case 3:
            return wav_trumpet;
        break;
        case 4:
            return wav_violin;
        break;
        case 0:
        default:
            return wav_flute8;
        break;
    }
}

void wave_organ_set_voice_on(uint8_t voiceIndex)
{
    const int16_t* voiceLut = wave_organ_get_voice_lut(voiceIndex);

    for (uint16_t lutIndex = 0; lutIndex < LUT_SIZE; lutIndex++)
    {
        wav_lut[lutIndex] += voiceLut[lutIndex];
    }

    wav_voices_on++;
}

void wave_organ_set_voice_off(uint8_t voiceIndex)
{
    const int16_t* voiceLut = wave_organ_get_voice_lut(voiceIndex);

    for (uint16_t lutIndex = 0; lutIndex < LUT_SIZE; lutIndex++)
    {
        wav_lut[lutIndex] -= voiceLut[lutIndex];
    }

    wav_voices_on--;
}
