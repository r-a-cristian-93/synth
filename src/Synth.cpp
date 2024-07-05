#include "AudioDriver.h"
#include "MidiDriver.h"
#include <OrganEngine/Config.h>
#include <OrganEngine/OrganOscillator.h>
#include <OrganEngine/RotarySpeaker.h>
#include <OrganEngine/WaveTables.h>
#include <OrganEngine/NoteManager.h>
#include <FmSynth/FmSynth.h>
#include <iostream>

int main()
{
    waveforms_initialize();
    organ_oscillator_initialize();
    rotary_speaker_initialize();
    fm_synth_init();

    midi_init();
    audio_driver_init();

    // Wait for user input (you can adjust this as needed)
    std::cout << "Press ESC to exit..." << std::endl;
    getchar();

    audio_driver_close();
    midi_close();

    return 0;
}