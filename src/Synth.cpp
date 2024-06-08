#include "OrganSynthesizer.h"

int main()
{
    osc_init();

    for (int i = 3; i < DRAWBARS_COUNT; i++)
    {
        osc_set_drawbar_amplitude(i, 0.0);
    }

    midi_init();
    audio_driver_init();

    // Wait for user input (you can adjust this as needed)
    std::cout << "Press ESC to exit..." << std::endl;
    getchar();

    audio_driver_close();
    midi_close();

    return 0;
}