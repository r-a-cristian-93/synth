#include "RotarySpeakerEffect.h"
#include "Config.h"

#include "BerVibrato/BerVibrato.h"

BerVibrato rotarySpeakerEffect;

void rotary_speaker_effect_init()
{
    rotarySpeakerEffect.initialize(SAMPLE_RATE);
    rotarySpeakerEffect.setFrequency(6.0);
    rotarySpeakerEffect.setDepth(0.1);
}

float rotary_speaker_effect_process_sample(float input) {
    return rotarySpeakerEffect.processOneSample(input);
}