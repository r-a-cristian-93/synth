#define MINIAUDIO_IMPLEMENTATION

#include <OrganEngine/RotarySpeaker.h>
#include <OrganEngine/NoteManager.h>
#include "OrganSynthesizer.h"
#include <FmSynth/FmSynth.h>
#include <WaveOrgan/WaveOrgan.h>
#include <DrumMachine/DrumMachine.h>
#include <DrumMachine/Sequencer.h>

void generateSamples(ma_device* pDevice, float* pInput, float* pOutput, ma_uint32 frameCount)
{
    reset_tonewheel_amplitude();
    set_tonewheels_amplitude();

    float* out = (float*) calloc(frameCount * 2, sizeof(float));
    float* const out_initial = out;

    float sample = 0;

    for (int iFrame = 0; iFrame < frameCount; iFrame++)
    {
        sample = organ_oscillator_generate_sample();

        sample = ((float)  rotary_speaker_process_sample(sample)) / (MAX_AMPLITUDE);

        *out++ = sample;
        *out++ = sample;
    }

    memcpy(pOutput, out_initial, frameCount * 2 *sizeof(float));
    free(out_initial);
}

void generateFmSynthSamples(ma_device* pDevice, float* pInput, float* pOutput, ma_uint32 frameCount)
{
    float* out = (float*) calloc(frameCount * 2, sizeof(float));
    float* const out_initial = out;

    float sample = 0;

    for (int iFrame = 0; iFrame < frameCount; iFrame++)
    {
        // sample = ((float) (fm_synth_generate_sample())) / (MAX_AMPLITUDE);
        sample = ((float) (fm_synth_generate_sample()) ) / (0xFFFF);

        *out++ = sample;
        *out++ = sample;
    }

    memcpy(pOutput, out_initial, frameCount * 2 *sizeof(float));
    free(out_initial);
}


void generateWaveOrganSamples(ma_device* pDevice, float* pInput, float* pOutput, ma_uint32 frameCount)
{
    float* out = (float*) calloc(frameCount * 2, sizeof(float));
    float* const out_initial = out;

    float sample = 0;

    for (int iFrame = 0; iFrame < frameCount; iFrame++)
    {
        sample = wave_organ_generate_sample();

        sample = ((float)  rotary_speaker_process_sample(sample)) / (MAX_AMPLITUDE);

        *out++ = sample;
        *out++ = sample;
    }

    memcpy(pOutput, out_initial, frameCount * 2 *sizeof(float));
    free(out_initial);
}

void generateDrumMachineSamples(ma_device* pDevice, float* pInput, float* pOutput, ma_uint32 frameCount)
{
    float* out = (float*) calloc(frameCount * 2, sizeof(float));
    float* const out_initial = out;

    float sample = 0;

    for (int iFrame = 0; iFrame < frameCount; iFrame++)
    {
        sequencer_tick();

        sample = wave_organ_generate_sample();
        sample = rotary_speaker_process_sample(sample);
        sample += drum_machine_generate_sample();

        sample = ((float) sample) / MAX_AMPLITUDE;
        *out++ = sample;
        *out++ = sample;
    }

    memcpy(pOutput, out_initial, frameCount * 2 *sizeof(float));
    free(out_initial);
}

void dataCallback(ma_device *pDevice, void *pOutput, const void *pInput, ma_uint32 frameCount)
{
    // generateSamples(pDevice, (float *)pInput, (float *)pOutput, frameCount);
    // generateFmSynthSamples(pDevice, (float *)pInput, (float *)pOutput, frameCount);
    // generateWaveOrganSamples(pDevice, (float *)pInput, (float *)pOutput, frameCount);
    generateDrumMachineSamples(pDevice, (float *)pInput, (float *)pOutput, frameCount);
}