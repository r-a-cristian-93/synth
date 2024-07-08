#include <FmSynth/FmSynth.h>
#include <iostream>
#include "Profiler.h"
#include <chrono>

int main()
{
    fm_synth_init();

    const int frameCount = 1000000;

    for (int note = 81; note < 121; note++)
    {
        for (int channel = 1; channel < 6; channel++)
        {
            fm_synth_note_on(note, channel);
        }
    }

    std::cout << "Starting test" << std::endl;

    using clock = std::chrono::high_resolution_clock;
    auto start = clock::now();

    for (int frame = 0; frame < frameCount; frame++)
    {
        fm_synth_generate_sample();
    }

    auto end = clock::now();
    std::chrono::duration<double> duration = end - start;
    std::cout << "Execution time: " << duration.count() << " seconds" << std::endl;

    return 0;
}