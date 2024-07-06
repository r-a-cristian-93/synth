#include <OrganEngine/Config.h>
#include <OrganEngine/OrganOscillator.h>
#include <OrganEngine/RotarySpeaker.h>
#include <OrganEngine/WaveTables.h>
#include <OrganEngine/NoteManager.h>
#include "../src/OrganSynthesizer.h"
#include <FmSynth/FmSynth.h>
#include <iostream>
#include "Profiler.h"
#include <chrono>

int main()
{
    waveforms_initialize();
    organ_oscillator_initialize();
    rotary_speaker_initialize();
    fm_synth_init();

    const ma_uint32 frameCount = 10000000;
    float* buffer = (float*) calloc(frameCount * 2, sizeof(float));

    for (int i = 13; i < 13+61; i++)
    {
        note_on(i);
    }

    using clock = std::chrono::high_resolution_clock;
    auto start = clock::now();

    dataCallback(0, buffer, 0, frameCount);

    auto end = clock::now();
    std::chrono::duration<double> duration = end - start;
    std::cout << "Execution time: " << duration.count() << " seconds" << std::endl;

    free(buffer);

    return 0;
}