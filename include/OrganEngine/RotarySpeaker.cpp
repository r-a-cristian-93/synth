#include "RotarySpeaker.h"

float       rotarySpeaker_depth;
int         rotarySpeaker_additionalDelay = 3;
RingBuffer  rotarySpeaker_ringBuffer;

float       rotarySpeaker_lfoPhase;
Parameter   rotarySpeaker_lfoPhaseIncrement
{
    ROTARY_SPEAKER_FAST,
    ROTARY_SPEAKER_FAST,
    0.001,
    ROTARY_SPEAKER_FAST,
    0.0,
    0.01 / SAMPLE_RATE
};

void rotary_speaker_initialize()
{
	rotarySpeaker_ringBuffer.resize(BASE_DELAY_SEC * SAMPLE_RATE * 2);
	rotary_speaker_set_depth(0.5);
	rotary_speaker_set_velocity_fast();
}

void rotary_speaker_set_depth(float depth)
{
	if (depth < 0)
		rotarySpeaker_depth = 0;
	else if (depth > 1)
		rotarySpeaker_depth = 1;
	else
		rotarySpeaker_depth = depth;
}

void rotary_speaker_set_velocity_fast() {
    rotarySpeaker_lfoPhaseIncrement.setValue(ROTARY_SPEAKER_FAST);
}

void rotary_speaker_set_velocity_slow() {
    rotarySpeaker_lfoPhaseIncrement.setValue(ROTARY_SPEAKER_SLOW);
}

void rotary_speaker_set_velocity_off() {
    rotarySpeaker_lfoPhaseIncrement.setValue(ROTARY_SPEAKER_OFF);
}