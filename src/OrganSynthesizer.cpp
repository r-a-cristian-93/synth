#define MINIAUDIO_IMPLEMENTATION

#include <OrganEngine/RotarySpeaker.h>
#include <OrganEngine/NoteManager.h>
#include "OrganSynthesizer.h"

void generateSamples(ma_device* pDevice, float* pInput, float* pOutput, ma_uint32 frameCount)
{
    organ_oscillator_update();
    rotary_speaker_parameters_update();

    Note note;
    float* out = (float*) calloc(frameCount * 2, sizeof(float));
    float* out2 = out;

    for (int noteIndex = 0; noteIndex < MAX_NOTES; noteIndex++)
    {
        note = notesList[noteIndex];

        if (note.envelope.getState() != ADSR_IDLE)
        {
            float sample = 0;
            out = out2;

            for (int iFrame = 0; iFrame < frameCount; iFrame++)
            {
                sample = (float) (organ_oscillator_generate_sample(note)) / (0xFFFFF);
                *out++ += sample;
                *out++ += sample;
            }

            notesList[noteIndex] = note;
        }
    }

    // float sample = 0;
    // out = out2;

    // // apply rotary speaker effect and mix with original

    // for (int iFrame = 0; iFrame < frameCount; iFrame++)
    // {
    //     sample = rotary_speaker_process_sample(*out);
    //     *out++ += (sample);
    //     *out++ = (sample);
    // }

    memcpy(pOutput, out2, frameCount * 2 *sizeof(float));
    free(out2);
}

void dataCallback(ma_device *pDevice, void *pOutput, const void *pInput, ma_uint32 frameCount)
{
    generateSamples(pDevice, (float *)pInput, (float *)pOutput, frameCount);
}