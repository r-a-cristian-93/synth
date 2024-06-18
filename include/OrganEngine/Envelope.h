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

// Must use 32bit in order to properly reach 16bit limit values
struct EnvelopeSettings
{
    int32_t attackRate;
    int32_t decayRate;
    int32_t sustainLevel;
    int32_t releaseRate;
};

extern EnvelopeSettings envelopeSettings;

class Envelope
{
    AdsrState state = ADSR_IDLE;
    int32_t amplitudeValue = 0;

public:
    void noteOn();
    void noteOff();

    __attribute__((always_inline)) inline
    uint16_t getAmplitude()
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
                if (amplitudeValue <= 0) {
                    amplitudeValue = 0;
                    state = ADSR_IDLE;
                }
            break;
        }

        return (int16_t) amplitudeValue;
    }

    __attribute__((always_inline)) inline
    const AdsrState& getState()
    {
        return state;
    }
};

#endif