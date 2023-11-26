#ifndef ENVELOPE_ADSR_H
#define ENVELOPE_ADSR_H

#include "Common.h"

struct EnvelopeADSR
{
    double dAttackDuration = 0.002;
    double dDecayDuration = 0.01;
    double dStartAmplitude = 1.0;
    double dSustainAmplitude = 0.8;
    double dReleaseDuration = 0.002;

    double dTriggerOnTime = 0.0;
    double dTriggerOffTime = 0.0;

    bool bNoteOn = false;

    EnvelopeADSR(const double dTime)
    {
        NoteOn(dTime);
    }

    double GetAmplitude(const double dTime) const
    {
        double dAmplitude = 0.0;

        if (bNoteOn)
        {
            double dLifeTime = dTime - dTriggerOnTime;

            // Attack
            if (dLifeTime <= dAttackDuration)
                dAmplitude = (dLifeTime / dAttackDuration) * dStartAmplitude;

            // Decay
            if (dLifeTime > dAttackDuration && dLifeTime <= (dAttackDuration + dDecayDuration))
                dAmplitude = ((dLifeTime - dAttackDuration) / dDecayDuration) * (dSustainAmplitude - dStartAmplitude) + dStartAmplitude;

            // Sustain
            if (dLifeTime > dAttackDuration + dDecayDuration)
                dAmplitude = dSustainAmplitude;
        }
        else
        {
            dAmplitude = ((dTime - dTriggerOffTime) / dReleaseDuration) * (0.0 - dSustainAmplitude) + dSustainAmplitude;
        }

        if (dAmplitude < PARAM_LOWEST_VALUE)
            dAmplitude = 0.0;

        return dAmplitude;
    }

    void NoteOn(const double dTime)
    {
        dTriggerOnTime = dTime;
        bNoteOn = true;
    }

    void NoteOff(const double dTime)
    {
        dTriggerOffTime = dTime;
        bNoteOn = false;
    }
};

#endif