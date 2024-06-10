#define MINIAUDIO_IMPLEMENTATION

#include "OrganSynthesizer.h"
#include "RotarySpeakerEffect.h"
#include "Lfo.h"

#include "SharedResources.h"

const float alpha = 0.4;

void generateSamples(ma_device* pDevice, float* pInput, float* pOutput, ma_uint32 frameCount)
{
    for (ma_uint32 iFrame = 0; iFrame < frameCount; iFrame++)
    {
        double sample = 0;

        lfo_update();
        organ_oscillator_update();

        {
            const std::lock_guard<std::mutex> lock(notesMutex);

            for (Note &note : notesList)
            {
                sample += organ_oscillator_generate_sample(note);
            }
        }

        // LPF
        // sample = (1 - alpha) * sample + alpha * *pOutput;

        // Vibrato
        sample = rotary_speaker_effect_process_sample(sample);

        // Limit volume so we won't blow up speakers
        if (sample > MAX_AMPLITUDE)
            sample = MAX_AMPLITUDE;
        if (sample < -MAX_AMPLITUDE)
            sample = -MAX_AMPLITUDE;


        // Add sample to left channel
        *pOutput++ = (float)sample;
        // Add sample to right channel
        *pOutput++ = (float)sample;
    }
}

void clearSilencedNotes()
{
    const std::lock_guard<std::mutex> lock(notesMutex);

    for (auto it = notesList.begin(); it != notesList.end(); it++)
    {
        // Remove one by one in the order they were added
        if (std::abs(it->envelope.state) == ADSR_IDLE)
        {
            notesList.erase(it++);
            break;
        }
    }
}

void dataCallback(ma_device *pDevice, void *pOutput, const void *pInput, ma_uint32 frameCount)
{
    generateSamples(pDevice, (float *)pInput, (float *)pOutput, frameCount);
    clearSilencedNotes();
}