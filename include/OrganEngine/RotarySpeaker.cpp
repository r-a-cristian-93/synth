#include "RotarySpeaker.h"

// From BerVibrato
// https://github.com/Bershov/Vibrato-effect/tree/master

#define ROTARY_SPEAKER_FAST (6.0 * PHASE_LUT_SCALE_FACTOR)
#define ROTARY_SPEAKER_SLOW (2.0 * PHASE_LUT_SCALE_FACTOR)
#define ROTARY_SPEAKER_OFF (0.0)

#define MAX_ROTATING_FREQUENCY_HZ (10.0)
#define MAX_MIDI_VALUE (127)


float       rotarySpeaker_additionalDelay = 3;
RingBuffer  rotarySpeaker_ringBuffer;

float       rotarySpeaker_lfoPhase;
float       rotarySpeaker_lfoPhaseIncrement = ROTARY_SPEAKER_FAST;
float       rotarySpeaket_depth;

void rotary_speaker_initialize()
{
	rotarySpeaker_ringBuffer.resize(BASE_DELAY_SEC * SAMPLE_RATE * 2);
	rotary_speaker_set_speed(16);
	rotary_speaker_set_depth(0);
}

void rotary_speaker_set_speed(uint8_t midiParam)
{
	rotarySpeaker_lfoPhaseIncrement = midiParam * (MAX_ROTATING_FREQUENCY_HZ / MAX_MIDI_VALUE) * PHASE_LUT_SCALE_FACTOR;
}

void rotary_speaker_set_depth(uint8_t midiParam)
{
	rotarySpeaket_depth = midiParam / MAX_MIDI_VALUE + 1.0;
}

