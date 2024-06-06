#ifndef ENVELOPE_ADSR_H
#define ENVELOPE_ADSR_H

#include "Config.h"

enum ADSR_STATE{
    ADSR_IDLE = 0,
    ADSR_ATTACK,
    ADSR_DECAY,
    ADSR_SUSTAIN,
    ADSR_RELEASE
};

struct EnvelopeAdsr
{
    bool bNoteOn = true;
    ADSR_STATE state = ADSR_ATTACK;

    float amplitudeValue;

    float attackRate = 10.0 / SAMPLE_RATE;
    float decayRate = 0.01 / SAMPLE_RATE;
    float sustainLevel = 0.8;
    float releaseRate = 100.0 / SAMPLE_RATE;

    EnvelopeAdsr()
    {
        NoteOn();
    }

    float getAmplitude() {
        switch (state) {
            case ADSR_IDLE:
                break;
            case ADSR_ATTACK:
                amplitudeValue += attackRate;
                if (amplitudeValue >= 1.0) {
                    amplitudeValue = 1.0;
                    state = ADSR_DECAY;
                }
                break;
            case ADSR_DECAY:
                amplitudeValue -= decayRate;
                if (amplitudeValue <= sustainLevel) {
                    amplitudeValue = sustainLevel;
                    state = ADSR_SUSTAIN;
                }
                break;
            case ADSR_SUSTAIN:
                break;
            case ADSR_RELEASE:
                amplitudeValue -= releaseRate;
                if (amplitudeValue <= 0.0) {
                    amplitudeValue = 0.0;
                    state = ADSR_IDLE;
                }
        }

        return amplitudeValue;
    }

    void NoteOn()
    {
        bNoteOn = true;
        state = ADSR_ATTACK;
    }

    void NoteOff()
    {
        bNoteOn = false;
        state = ADSR_RELEASE;
    }
};

#endif