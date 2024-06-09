#include "VibratoEffect.h"
#include "Config.h"

#include "BerVibrato/BerVibrato.h"

BerVibrato vibratoEffect;

void vibrato_effect_init()
{
    vibratoEffect.initialize(SAMPLE_RATE);
    vibratoEffect.setFrequency(6.0);
    vibratoEffect.setDepth(0.1);
}

float vibrato_effect_process_sample(float input) {
    return vibratoEffect.processOneSample(input);
}