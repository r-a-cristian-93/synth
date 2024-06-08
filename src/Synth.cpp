#define MINIAUDIO_IMPLEMENTATION
#include <iostream>
#include <cmath>
#include <signal.h>
#include <bitset>
#include <list>
#include <miniaudio/miniaudio.h>
#include <RtMidi/RtMidi.h>
#include <nanomidi/decoder.h>
#include <nanomidi/msgprint.h>
#include <mutex>
#include <string>

#include "Config.h"
#include "Parameter.h"
#include "Envelope.h"
#include "MidiManager.h"
#include "Note.h"
#include "OrganOscillator.h"

double g_time = 0.0;

std::list<Note> notes_list;
std::mutex notesMutex;

void generateSamples(ma_device *pDevice, void *pOutput, ma_uint32 frameCount)
{
    float *pOutputF32 = (float *)pOutput;

    for (ma_uint32 iFrame = 0; iFrame < frameCount; iFrame++)
    {
        double sample = 0;
        double noteSample = 0;

        osc_update();

        {
            // Get a lock on notes_list
            const std::lock_guard<std::mutex> lock(notesMutex);

            for (Note &note : notes_list)
            {
                noteSample = osc_generate_sample(note);
                sample += noteSample;
            }
        }

        // Limit volume so we won't blow up speakers
        if (sample > MAX_AMPLITUDE)
            sample = MAX_AMPLITUDE;
        if (sample < -MAX_AMPLITUDE)
            sample = -MAX_AMPLITUDE;

        // Add sample to left channel
        *pOutputF32++ = (float)sample;
        // Add sample to right channel
        *pOutputF32++ = (float)sample;

        // Advance time
        g_time += 1.0 / (double)pDevice->playback.internalSampleRate;

    }
}

void clearSilencedNotes()
{
    const std::lock_guard<std::mutex> lock(notesMutex);

    for (auto it = notes_list.begin(); it != notes_list.end(); it++)
    {
        // Remove one by one in the order they were added
        if (std::abs(it->envelope.state) == ADSR_IDLE)
        {
            notes_list.erase(it++);
            break;
        }
    }
}

void dataCallback(ma_device *pDevice, void *pOutput, const void *pInput, ma_uint32 frameCount)
{
    clearSilencedNotes();
    generateSamples(pDevice, pOutput, frameCount);
}

void decode_message(double deltatime, std::vector<unsigned char> *buffer, void *userData)
{
    unsigned int nBytes = buffer->size();
    struct midi_istream istream;

    // Nanomidi does not read from std::vector so send the address of the first element
    midi_istream_from_buffer(&istream, &buffer->at(0), nBytes);
    struct midi_message *message = midi_decode(&istream);
    print_msg(message);

    switch (message->type)
    {
    case MIDI_TYPE_NOTE_ON:
    {
        const std::lock_guard<std::mutex> lock(notesMutex);

        notes_list.emplace_back(Note{message->data.note_on.note});
    }
    break;

    case MIDI_TYPE_NOTE_OFF:
    {
        // Call NoteOff on first occurence
        const std::lock_guard<std::mutex> lock(notesMutex);

        for (Note &note : notes_list)
        {
            if (note.midiNote == message->data.note_off.note && note.envelope.state != ADSR_IDLE)
            {
                EnvelopeAdsr_NoteOff(&note.envelope);
                break;
            }
        }
    }
    break;

    case MIDI_TYPE_CONTROL_CHANGE:
        uint8_t controller = message->data.control_change.controller;
        uint8_t value = message->data.control_change.value;

        std::cout << "Controller: " << (int)controller << " Value: " << (int)value << std::endl;

        if (is_drawbar_controller(controller))
            osc_set_drawbar_amplitude(get_drawbar_id(controller), value / 127.0f);
        // if (is_vibrato_controller(controller))
        // {
        //     double vibrato_value;

        //     if (value == 0) vibrato_value = vibrato_frequency.min_value;
        //     else vibrato_value = vibrato_frequency.max_value;

        //     set_parameter_value(
        //         vibrato_frequency,
        //         vibrato_value
        //     );
        // }
        break;
    }
}

int main()
{
    osc_init();
    for (int i = 3; i < DRAWBARS_COUNT; i++)
    {
        osc_set_drawbar_amplitude(i, 0.0);
    }

    ma_result result;
    ma_context context;
    ma_device device;
    RtMidiIn *midiin = 0;

    // Initialize midi input
    try
    {
        midiin = new RtMidiIn();
    }
    catch (RtMidiError &error)
    {
        // Handle the exception here
        error.printMessage();
    }

    // Check inputs.
    unsigned int nPorts = midiin->getPortCount();
    std::cout << "\nThere are " << nPorts << " MIDI input sources available.\n";
    std::string portName;
    for (unsigned int i = 0; i < nPorts; i++)
    {
        try
        {
            portName = midiin->getPortName(i);
        }
        catch (RtMidiError &error)
        {
            error.printMessage();
        }
        std::cout << "  Input Port #" << i + 1 << ": " << portName << '\n';
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

    // notes_list.push_back(Note{organ, 45, 127});

    // Set port
    midiin->openPort(port);
    midiin->setCallback(&decode_message);

    // Don't ignore sysex, timing, or active sensing messages.
    midiin->ignoreTypes(true, true, true);

    // Initialize miniaudio context
    result = ma_context_init(NULL, 0, NULL, &context);
    if (result != MA_SUCCESS)
    {
        std::cerr << "Failed to initialize miniaudio context." << std::endl;
        return 1;
    }

    ma_device_config config = ma_device_config_init(ma_device_type_playback);
    config.playback.format = ma_format_f32; // Set to ma_format_unknown to use the device's native format.
    config.playback.channels = 2;           // Set to 0 to use the device's native channel count.
    config.sampleRate = 44100;              // Set to 0 to use the device's native sample rate.
    config.dataCallback = dataCallback;     // This function will be called when miniaudio needs more data.
    config.pUserData = NULL;                // Can be accessed from the device object (device.pUserData).

    result = ma_device_init(NULL, &config, &device);
    if (result != MA_SUCCESS)
    {
        std::cerr << "Failed to initialize playback device." << std::endl;
        ma_context_uninit(&context);
        return 1;
    }

    result = ma_device_start(&device);
    if (result != MA_SUCCESS)
    {
        std::cerr << "Failed to start playback device." << std::endl;
        ma_device_uninit(&device);
        ma_context_uninit(&context);
        return 1;
    }

    std::cout << "Internal sample rate: " << device.playback.internalSampleRate << std::endl;
    std::cout << "PLayback device name: " << device.playback.name << std::endl;

    // Wait for user input (you can adjust this as needed)
    std::cout << "Press ESC to exit..." << std::endl;
    getchar();

    // while (true)
    // {
    //     int input = getchar();

    //     if (input == 27)
    //         break;

    //     switch (input)
    //     {
    //     case 'd':
    //         increase_value(vibrato_amplitude);
    //         break;
    //     case 'c':
    //         decrease_value(vibrato_amplitude);
    //         break;
    //     case 'f':
    //         increase_value(vibrato_frequency);
    //         break;
    //     case 'v':
    //         decrease_value(vibrato_frequency);
    //         break;
    //     }

    //     for (int i = 0; i < DRAWBARS_COUNT; i++)
    //         std::cout << (i + 1) << " " << drawbar_amplitude[i].current_value << " " << drawbar_amplitude[i].target_value << std::endl;

    //     std::cout << std::endl;
    //     std::cout << "VIBRATO_AMPLITUDE:  " << vibrato_amplitude.current_value << std::endl;
    //     std::cout << "VIBRATO_FREQUENCY:  " << vibrato_frequency.current_value << std::endl;
    //     std::cout << "NOTES_LIST:  " << notes_list.size() << std::endl;
    //     std::cout << std::endl;
    // }

    ma_device_uninit(&device);
    ma_context_uninit(&context);

    delete midiin;

    return 0;
}