#include <WaveOrgan/WaveOrgan.h>
#include <iostream>
#include <chrono>

int main()
{
    wave_organ_init();

    const int frameCount = 1000000;

    for (int note = 36; note < 97; note++)
    {
        wav_organ_note_on(note);
    }

    std::cout << "Starting test" << std::endl;

    using clock = std::chrono::high_resolution_clock;
    auto start = clock::now();

    for (int frame = 0; frame < frameCount; frame++)
    {
        wave_organ_generate_sample();
    }

    auto end = clock::now();
    std::chrono::duration<double> duration = end - start;
    std::cout << "Execution time: " << duration.count() << " seconds" << std::endl;

    return 0;
}