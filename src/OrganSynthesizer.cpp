#define MINIAUDIO_IMPLEMENTATION

#include <SynthEngine/RotarySpeaker.h>
#include <SynthEngine/SharedResources.h>
#include "OrganSynthesizer.h"

void generateSamples(ma_device* pDevice, float* pInput, float* pOutput, ma_uint32 frameCount)
{
    for (ma_uint32 iFrame = 0; iFrame < frameCount; iFrame++)
    {
        double sample = 0;

        organ_oscillator_update();
        rotary_speaker_parameters_update();

        {
            const std::lock_guard<std::mutex> lock(notesMutex);

            for (Note &note : notesList)
            {
                sample += organ_oscillator_generate_sample(note);
            }
        }

        sample = rotary_speaker_process_sample(sample);

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
        if (std::abs(it->envelope.getState()) == ADSR_IDLE)
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