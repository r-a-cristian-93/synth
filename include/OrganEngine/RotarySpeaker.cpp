#include "RotarySpeaker.h"

float       rotarySpeaker_depth;
int         rotarySpeaker_additionalDelay = 3;
RingBuffer  rotarySpeaker_ringBuffer;

float       rotarySpeaker_lfoPhase;
float       rotarySpeaker_lfoPhaseIncrement = ROTARY_SPEAKER_FAST;

void rotary_speaker_initialize()
{
	rotarySpeaker_ringBuffer.resize(BASE_DELAY_SEC * SAMPLE_RATE * 2);
	rotary_speaker_set_depth(127);
	rotary_speaker_set_speed(127);
}

void rotary_speaker_set_depth(uint8_t midiParam)
{
	rotarySpeaker_depth = midiParam * (1.0 / 127.0);
}

void rotary_speaker_set_speed(uint8_t midiParam) {
	rotarySpeaker_lfoPhaseIncrement = midiParam * (10.0 / 127.0) * PHASE_LUT_SCALE_FACTOR;
}

