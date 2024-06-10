#include "AudioDriver.h"
#include "MidiDriver.h"
#include "OrganSynthesizer.h"
#include "RotarySpeaker.h"

int main()
{
    organ_oscillator_init();
    rotarySpeaker.initialize(SAMPLE_RATE);

    for (int i = 3; i < DRAWBARS_COUNT; i++)
    {
        organ_oscillator_set_drawbar_amplitude(i, 0.0);
    }

    organ_oscillator_set_drawbar_amplitude(6, 0.1);
    organ_oscillator_set_drawbar_amplitude(7, 0.1);


    midi_init();
    audio_driver_init();

    // Wait for user input (you can adjust this as needed)
    std::cout << "Press ESC to exit..." << std::endl;
    getchar();

    audio_driver_close();
    midi_close();

    return 0;
}