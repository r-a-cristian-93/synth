
#include <DrumMachine/Sequencer.h>
#include <iostream>
#include <chrono>

int main()
{
    sequencer_init();
    sequencer_set_bpm(300);
    sequencer_set_sequence(5);


    const int frameCount = 1000000000;

    std::cout << "Starting test" << std::endl;

    using clock = std::chrono::high_resolution_clock;
    auto start = clock::now();

    for (int frame = 0; frame < frameCount; frame++)
    {
        sequencer_tick();
    }

    auto end = clock::now();
    std::chrono::duration<double> duration = end - start;
    std::cout << "Execution time: " << duration.count() << " seconds" << std::endl;

    return 0;
}