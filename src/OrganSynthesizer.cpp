#define MINIAUDIO_IMPLEMENTATION

#include <OrganEngine/RotarySpeaker.h>
#include <OrganEngine/NoteManager.h>
#include "OrganSynthesizer.h"

void generateSamples(ma_device* pDevice, float* pInput, float* pOutput, ma_uint32 frameCount)
{
    rotary_speaker_parameters_update();

    float* out = (float*) calloc(frameCount * 2, sizeof(float));
    float* const out_initial = out;

    float sample = 0;

    for (int iFrame = 0; iFrame < frameCount; iFrame++)
    {
        organ_oscillator_update();
        sample = organ_oscillator_generate_sample();

        sample = ((float)  rotary_speaker_process_sample(sample)) / (MAX_AMPLITUDE);

        *out++ = sample;
        *out++ = sample;
    }

    memcpy(pOutput, out_initial, frameCount * 2 *sizeof(float));
    free(out_initial);
}

void dataCallback(ma_device *pDevice, void *pOutput, const void *pInput, ma_uint32 frameCount)
{
    generateSamples(pDevice, (float *)pInput, (float *)pOutput, frameCount);
}