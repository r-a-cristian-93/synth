#include "RotarySpeaker.h"

#define ROTARY_SPEAKER_FAST (6.0 * PHASE_LUT_SCALE_FACTOR)
#define ROTARY_SPEAKER_SLOW (2.0 * PHASE_LUT_SCALE_FACTOR)
#define ROTARY_SPEAKER_OFF (0.0)

#define MAX_ROTATING_FREQUENCY_HZ (10.0)
#define MAX_MIDI_VALUE (127)


int         rotarySpeaker_additionalDelay = 3;
RingBuffer  rotarySpeaker_ringBuffer;

float       rotarySpeaker_lfoPhase;
float       rotarySpeaker_lfoPhaseIncrement = ROTARY_SPEAKER_FAST;

void rotary_speaker_initialize()
{
	rotarySpeaker_ringBuffer.resize(BASE_DELAY_SEC * SAMPLE_RATE * 2);
	rotary_speaker_set_speed(20);
}

void rotary_speaker_set_speed(uint8_t midiParam) {
	rotarySpeaker_lfoPhaseIncrement = midiParam * (MAX_ROTATING_FREQUENCY_HZ / MAX_MIDI_VALUE) * PHASE_LUT_SCALE_FACTOR;
}

