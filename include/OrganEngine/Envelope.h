#ifndef ENVELOPE_ADSR_H
#define ENVELOPE_ADSR_H

#include <OrganEngine/Config.h>

enum AdsrState
{
    ADSR_IDLE = 0,
    ADSR_ATTACK,
    ADSR_DECAY,
    ADSR_SUSTAIN,
    ADSR_RELEASE
};

struct EnvelopeSettings
{
    float attackRate;
    float decayRate;
    float sustainLevel;
    float releaseRate;
};

extern EnvelopeSettings envelopeSettings;

class Envelope
{
    AdsrState state = ADSR_IDLE;
    float amplitudeValue = 0.0;

public:
    void noteOn();
    void noteOff();

    __attribute__((always_inline)) inline
    float getAmplitude()
    {
        switch (state) {
            case ADSR_IDLE:
                break;
            case ADSR_ATTACK:
                amplitudeValue += envelopeSettings.attackRate;
                if (amplitudeValue >= MAX_AMPLITUDE) {
                    amplitudeValue = MAX_AMPLITUDE;
                    state = ADSR_DECAY;
                }
                break;
            case ADSR_DECAY:
                amplitudeValue -= envelopeSettings.decayRate;
                if (amplitudeValue <= envelopeSettings.sustainLevel) {
                    amplitudeValue = envelopeSettings.sustainLevel;
                    state = ADSR_SUSTAIN;
                }
                break;
            case ADSR_SUSTAIN:
                break;
            case ADSR_RELEASE:
                amplitudeValue -= envelopeSettings.releaseRate;
                if (amplitudeValue <= MIN_AMPLITUDE) {
                    amplitudeValue = MIN_AMPLITUDE;
                    state = ADSR_IDLE;
                }
        }

        return amplitudeValue;
    }

    __attribute__((always_inline)) inline
    const AdsrState& getState()
    {
        return state;
    }
};

#endif