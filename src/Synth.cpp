#define MINIAUDIO_IMPLEMENTATION
#include <iostream>
#include <cmath>
#include <signal.h>
#include <bitset>
#include <miniaudio/miniaudio.h>
#include <RtMidi/RtMidi.h>
#include <nanomidi/decoder.h>
#include <nanomidi/msgprint.h>

#include "Common.h"
#include "Parameter.h"

#define HARMONICS_COUNT 9
#define HARMONICS_AMPLITUDE_MAX 1.0
#define HARMONICS_AMPLITUDE_MIN 0.0
#define HARMONICS_AMPLITUDE_INCREMENT 0.02
#define MAX_POLYPHONY

Parameter drawbar_amplitude[HARMONICS_COUNT] = {Parameter()};
Parameter vibrato_amplitude{0.002, 0.002, 0.001, 0.1};
Parameter vibrato_frequency{0.8, 0.8, 0.01, 6.8, 0.8, 0.0001};
Parameter phase_shift{0.0, 0.0, 0.00001, 0.02};

double g_time = 0;
double g_amplitude = 0.1;

void (*current_instrument)(ma_device*, void*, const void*, ma_uint32);

struct Note {
    uint8_t value;
    uint8_t velocity;
};

std::vector<Note> notes_queue;
int notes[128];

bool is_drawbar_controller(uint8_t controller)
{
    if (controller > 69 && controller < 79)
        return true;
    else
        return false;
}

unsigned int get_drawbar_id(uint8_t controller)
{
    return controller - 70;
}

bool is_vibrato_controller(uint8_t controller)
{
    if (controller == 18)
        return true;
    else return false;
}

void play_harmonics(ma_device *pDevice, void *pOutput, const void *pInput, ma_uint32 frameCount)
{
    float *pOutputF32 = (float *)pOutput;

    for (ma_uint32 iFrame = 0; iFrame < frameCount; iFrame++)
    {
        double value = 0;
        //double trigger = (g_amplitude * sin(M_2PI * (note_frequency[note][0]) * g_time));

        update_parameter(vibrato_amplitude);
        update_parameter(vibrato_frequency);
        update_parameter(phase_shift);

        for (unsigned int note_index = 0; note_index < 128; note_index++)
        {
            if (notes[note_index]) 
            {
                for (int drawbar_index = 0; drawbar_index < HARMONICS_COUNT; drawbar_index++)
                {
                    update_parameter(drawbar_amplitude[drawbar_index]);

                    value +=
                        sin(
                            M_2PI * (note_frequency[note_index][drawbar_index]) * (g_time + phase_shift.current_value * drawbar_index)
                            + (vibrato_amplitude.current_value * note_frequency[note_index][drawbar_index] * sin(M_2PI * vibrato_frequency.current_value * g_time)) // vibrato
                        ) * drawbar_amplitude[drawbar_index].current_value * g_amplitude * 0.01;
                }
            }
        }
        if (value > 0.3)
            value = 0.3;

        // left channel
        *pOutputF32++ = (float)value;
        // right channel
        *pOutputF32++ = (float)value;

        g_time += 1.0 / (double)pDevice->playback.internalSampleRate;
    }
}

void DataCallback(ma_device *pDevice, void *pOutput, const void *pInput, ma_uint32 frameCount)
{
    play_harmonics(pDevice, pOutput, pInput, frameCount);
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
            notes_queue.push_back(
                Note{message->data.note_on.note, message->data.note_on.velocity}
            );

            notes[message->data.note_on.note] = 1;
            break;
        case MIDI_TYPE_NOTE_OFF:
            notes[message->data.note_off.note] = 0;
            break;
        case MIDI_TYPE_CONTROL_CHANGE:
            uint8_t controller = message->data.control_change.controller;
            uint8_t value = message->data.control_change.value;

            if (is_drawbar_controller(controller))
                set_parameter_value(
                    drawbar_amplitude[get_drawbar_id(controller)],
                    value / 127.0f
                );
            if (is_vibrato_controller(controller))
            {
                double vibrato_value;

                if (value == 0) vibrato_value = vibrato_frequency.min_value;
                else vibrato_value = vibrato_frequency.max_value;

                set_parameter_value(
                    vibrato_frequency,
                    vibrato_value
                );
            }
            break;
    }
}


int main()
{
    ma_result result;
    ma_context context;
    ma_device device;
    RtMidiIn *midiin = 0;
    std::vector<unsigned char> message;
    int nBytes, i;
    double stamp;

    current_instrument = play_harmonics;

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

    // Set port
    midiin->openPort(1);
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
    config.dataCallback = DataCallback;     // This function will be called when miniaudio needs more data.
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

    // Wait for user input (you can adjust this as needed)
    std::cout << "Press ESC to exit..." << std::endl;
    while (true)
    {
        int input = getchar();

        if (input == 27)
            break;

        switch (input)
        {
        case '1':
            increase_value(drawbar_amplitude[0]);
            break;
        case '2':
            increase_value(drawbar_amplitude[1]);
            break;
        case '3':
            increase_value(drawbar_amplitude[2]);
            break;
        case '4':
            increase_value(drawbar_amplitude[3]);
            break;
        case '5':
            increase_value(drawbar_amplitude[4]);
            break;
        case '6':
            increase_value(drawbar_amplitude[5]);
            break;
        case '7':
            increase_value(drawbar_amplitude[6]);
            break;
        case '8':
            increase_value(drawbar_amplitude[7]);
            break;
        case '9':
            increase_value(drawbar_amplitude[8]);
            break;

        case 'q':
            decrease_value(drawbar_amplitude[0]);
            break;
        case 'w':
            decrease_value(drawbar_amplitude[1]);
            break;
        case 'e':
            decrease_value(drawbar_amplitude[2]);
            break;
        case 'r':
            decrease_value(drawbar_amplitude[3]);
            break;
        case 't':
            decrease_value(drawbar_amplitude[4]);
            break;
        case 'y':
            decrease_value(drawbar_amplitude[5]);
            break;
        case 'u':
            decrease_value(drawbar_amplitude[6]);
            break;
        case 'i':
            decrease_value(drawbar_amplitude[7]);
            break;
        case 'o':
            decrease_value(drawbar_amplitude[8]);
            break;

        case 'a':
            increase_value(phase_shift);
            break;
        case 'z':
            decrease_value(phase_shift);
            break;
        case 'd':
            increase_value(vibrato_amplitude);
            break;
        case 'c':
            decrease_value(vibrato_amplitude);
            break;
        case 'f':
            increase_value(vibrato_frequency);
            break;
        case 'v':
            decrease_value(vibrato_frequency);
            break;
        }

        for (int i = 0; i < HARMONICS_COUNT; i++)
            std::cout << (i + 1) << " " << drawbar_amplitude[i].current_value << " " << drawbar_amplitude[i].target_value << std::endl;

        std::cout << std::endl;
        std::cout << "PHASE:  " << phase_shift.current_value << std::endl;
        std::cout << "VIBRATO_AMPLITUDE:  " << vibrato_amplitude.current_value << std::endl;
        std::cout << "VIBRATO_FREQUENCY:  " << vibrato_frequency.current_value << std::endl;
        std::cout << std::endl;
    }

    ma_device_uninit(&device);
    ma_context_uninit(&context);

    delete midiin;

    return 0;
}