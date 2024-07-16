#include "MidiDriver.h"

#include <RtMidi/RtMidi.h>
#include <nanomidi/decoder.h>
#include <nanomidi/msgprint.h>

#include <OrganEngine/NoteManager.h>
#include <OrganEngine/OrganOscillator.h>
#include <OrganEngine/MidiManager.h>
#include <FmSynth/FmSynth.h>
#include <WaveOrgan/WaveOrgan.h>
#include <OrganEngine/RotarySpeaker.h>
#include <DrumMachine/DrumMachine.h>

RtMidiIn *midiDevice = 0;

void decode_message(double deltatime, std::vector<unsigned char> *buffer, void *userData)
{
    unsigned int nBytes = buffer->size();
    struct midi_istream istream;

    // Nanomidi does not read from std::vector so send the address of the first element
    midi_istream_from_buffer(&istream, &buffer->at(0), nBytes);
    struct midi_message *message = midi_decode(&istream);
    print_msg(message);
    printf("%d", message->channel);

    switch (message->type)
    {
    case MIDI_TYPE_NOTE_ON:
    {
        note_on(message->data.note_on.note);
        fm_synth_note_on(message->data.note_on.note, message->channel);
        wav_organ_note_on(message->data.note_on.note);

        drum_machine_play(message->data.note_on.note, message->data.note_on.velocity);
    }
    break;

    case MIDI_TYPE_NOTE_OFF:
    {
        note_off(message->data.note_off.note);
        fm_synth_note_off(message->data.note_off.note, message->channel);
        wav_organ_note_off(message->data.note_off.note);
    }
    break;

    case MIDI_TYPE_CONTROL_CHANGE:
        uint8_t controller = message->data.control_change.controller;
        uint8_t value = message->data.control_change.value;

        std::cout << "Controller: " << (int)controller << " Value: " << (int)value << " " << std::endl;

        if (is_drawbar_controller(controller))
            organ_oscillator_set_drawbar_amplitude(controller, value);

        if (controller == MIDI_CC_BANK_MSB)
        {
            set_next_waveform();
            wave_organ_set_voice(value);

            // fm_synth_next_instrument();
        }

        if (controller == 20) {
            wave_organ_set_orchestra_volume(value);
        }

        if (controller == 21) {
            wave_organ_set_bass_volume(value);
        }

        if (controller == 10) {
            fm_synth_increment_fmInc(message->channel);
        }

        if (controller == 11) {
            fm_synth_decrement_fmInc(message->channel);
        }

        if (controller == 12) {
            fm_synth_increment_fmAsAe(message->channel);
        }

        if (controller == 13) {
            fm_synth_decrement_fmAsAe(message->channel);
        }

        if (controller == 92) {
            rotary_speaker_set_speed(value);
        }

        // if (controller == MIDI_CC_VIBRATO_FAST)
        //     organ_oscillator_set_vibrato_fast();

        // if (controller == MIDI_CC_VIBRATO_SLOW)
        //     organ_oscillator_set_vibrato_slow();

        // if (controller == MIDI_CC_VIBRATO_OFF)
        //     organ_oscillator_set_vibrato_off();

        break;
    }
}

int midi_init()
{
    // Initialize midi input
    try
    {
        midiDevice = new RtMidiIn();
    }
    catch (RtMidiError &error)
    {
        // Handle the exception here
        error.printMessage();
    }

    // Check inputs.
    unsigned int nPorts = midiDevice->getPortCount();
    std::cout << "\nThere are " << nPorts << " MIDI input sources available.\n";
    std::string portName;
    for (unsigned int i = 0; i < nPorts; i++)
    {
        try
        {
            portName = midiDevice->getPortName(i);
        }
        catch (RtMidiError &error)
        {
            error.printMessage();
        }
        std::cout << "  Input Port #" << i  << ": " << portName << '\n';
    }

    std::cout << "Pick midi input port: ";
    std::string inputString = "";
    int port = 0;

    getline(std::cin, inputString);

    try
    {
        port = std::stoi(inputString);
    }
    catch (std::invalid_argument)
    {
        return 1;
    }

    midiDevice->openPort(port);
    midiDevice->setCallback(&decode_message);
}


void midi_close()
{
    delete midiDevice;
}
