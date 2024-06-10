#include "RotarySpeakerEffect.h"
#include "Config.h"

#include "RotarySpeaker.h"

RotarySpeaker rotarySpeakerEffect;

void rotary_speaker_effect_init()
{
    rotarySpeakerEffect.initialize(SAMPLE_RATE);
    rotarySpeakerEffect.setFrequency(6.0);
    rotarySpeakerEffect.setDepth(0.1);
}

void rotary_speaker_effect_update()
{
    // rotarySpeakerEffect.update();
}

float rotary_speaker_effect_process_sample(float input) {
    return rotarySpeakerEffect.processOneSample(input);
}