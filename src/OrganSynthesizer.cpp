#define MINIAUDIO_IMPLEMENTATION

#include <OrganEngine/RotarySpeaker.h>
#include <OrganEngine/NoteManager.h>
#include "OrganSynthesizer.h"

void generateSamples(ma_device* pDevice, float* pInput, float* pOutput, ma_uint32 frameCount)
{
    rotary_speaker_parameters_update();

    Note note;
    float* out = (float*) calloc(frameCount * 2, sizeof(float));
    float* const out_initial = out;

    // for (int noteIndex = 0; noteIndex < MAX_NOTES; noteIndex++)
    // {
    //     note = notesList[noteIndex];

    //     if (note.envelope.getState() != ADSR_IDLE)
    //     {
    //         float sample = 0;
    //         out = out_initial;

    //         for (int iFrame = 0; iFrame < frameCount; iFrame++)
    //         {
    //             // Scale to float -1 to 1. Platform dependent
    //             sample = organ_oscillator_generate_sample(note);
    //             *out++ += sample;
    //             *out++ += sample;
    //         }

    //         notesList[noteIndex] = note;
    //     }
    // }

    for (int iFrame = 0; iFrame < frameCount; iFrame++)
    {
        organ_oscillator_update();
        float sample = 0;

        for (int noteIndex = 0; noteIndex < MAX_NOTES; noteIndex++)
        {
            note = notesList[noteIndex];

            if (note.envelope.getState() != ADSR_IDLE)
            {
                sample += organ_oscillator_generate_sample(note);

                notesList[noteIndex] = note;
            }
        }

        *out++ = sample;
        *out++ = sample;
    }

    float sample = 0;
    out = out_initial;

    // apply rotary speaker effect and mix with original

    for (int iFrame = 0; iFrame < frameCount; iFrame++)
    {
        sample = ((float)  rotary_speaker_process_sample(*out)) / (MAX_AMPLITUDE);

        // sample = (float) (*out) / (MAX_AMPLITUDE);
        *out++ = (sample);
        *out++ = (sample);
    }

    memcpy(pOutput, out_initial, frameCount * 2 *sizeof(float));
    free(out_initial);
}

void dataCallback(ma_device *pDevice, void *pOutput, const void *pInput, ma_uint32 frameCount)
{
    generateSamples(pDevice, (float *)pInput, (float *)pOutput, frameCount);
}