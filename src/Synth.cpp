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
double g_amplitude = 1.0;

struct EnvelopeADSR {
    double dAttackDuration = 0.1;
    double dDecayDuration = 0.1;
    double dStartAmplitude = 1.0;
    double dSustainAmplitude = 0.8;
    double dReleaseDuration = 0.01;

    double dTriggerOnTime = 0.0;
    double dTriggerOffTime = 0.0;

    bool bNoteOn = false;

    EnvelopeADSR(const double dTime)
    {
        NoteOn(dTime);
    }

    double GetAmplitude(const double dTime) const
    {
        double dAmplitude = 0.0;
        
        if (bNoteOn) 
        {
            double dLifeTime = dTime - dTriggerOnTime;

            // Attack
            if (dLifeTime <= dAttackDuration) 
                dAmplitude = (dLifeTime / dAttackDuration) * dStartAmplitude;
            
            // Decay
            if (dLifeTime > dAttackDuration && dLifeTime <= (dAttackDuration + dDecayDuration))
                dAmplitude = ((dLifeTime - dAttackDuration) / dDecayDuration)
                    * (dSustainAmplitude - dStartAmplitude)
                    + dStartAmplitude;
            
            // Sustain
            if (dLifeTime > dAttackDuration + dDecayDuration)
                dAmplitude = dSustainAmplitude;
        }
        else 
        {
            dAmplitude = ((dTime - dTriggerOffTime) / dReleaseDuration )
                * (0.0 - dSustainAmplitude)
                + dSustainAmplitude;
        }

        if (dAmplitude < 0.0001)
            dAmplitude = 0.0;

        return dAmplitude;
    }

    void NoteOn(const double dTime) 
    {
        dTriggerOnTime = dTime;
        bNoteOn = true;
    }

    void NoteOff(const double dTime)
    {
        dTriggerOffTime = dTime;
        bNoteOn = false;
    }
};


struct Note {
    uint8_t value = 0;
    EnvelopeADSR envelope;

    bool operator ==(const Note &other) const
    {
        return value == other.value;
    }
};

std::list<Note> notes_list;
std::mutex notesMutex;

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

        {
            const std::lock_guard<std::mutex> lock(notesMutex);

            for (Note const &note : notes_list)
            {
                for (int drawbar_index = 0; drawbar_index < HARMONICS_COUNT; drawbar_index++)
                {
                    update_parameter(drawbar_amplitude[drawbar_index]);

                    value +=
                        sin(
                            M_2PI * (note_frequency[note.value][drawbar_index]) * (g_time + phase_shift.current_value * drawbar_index)
                            + (vibrato_amplitude.current_value * note_frequency[note.value][drawbar_index] * sin(M_2PI * vibrato_frequency.current_value * g_time)) // vibrato
                        )
                            * drawbar_amplitude[drawbar_index].current_value * g_amplitude
                            * note.envelope.GetAmplitude(g_time)
                            * 0.03;
                }
            }
        }

        // Limit volume so we won't blow up speakers
        if (value > 1.0) value = 1.0;

        // left channel
        *pOutputF32++ = (float)value;
        // right channel
        *pOutputF32++ = (float)value;

        g_time += 1.0 / (double)pDevice->playback.internalSampleRate;
    }
}

void DataCallback(ma_device *pDevice, void *pOutput, const void *pInput, ma_uint32 frameCount)
{
    {
        const std::lock_guard<std::mutex> lock(notesMutex);

        // Remove silenced notes
        for (auto it = notes_list.begin(); it != notes_list.end(); it++)
        {
            if (it->envelope.GetAmplitude(g_time) <= 0 && !it->envelope.bNoteOn)
            {
                notes_list.erase(it++);
                break;
            }
        }
    }

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
        {
            const std::lock_guard<std::mutex> lock(notesMutex);

            notes_list.push_back(
                Note{message->data.note_on.note, EnvelopeADSR(g_time)}
            );
        }
        break;

        case MIDI_TYPE_NOTE_OFF:
        {
            // Call NoteOff on first occurence
            const std::lock_guard<std::mutex> lock(notesMutex);

            for (Note &note : notes_list)
            {
                if (note.value == message->data.note_off.note && note.envelope.bNoteOn)
                {
                    note.envelope.NoteOff(g_time);
                    break;
                }
            }
        }
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
    midiin->openPort(0);
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
        std::cout << "NOTES_LIST:  " << notes_list.size() << std::endl;
        std::cout << std::endl;
    }

    ma_device_uninit(&device);
    ma_context_uninit(&context);

    delete midiin;

    return 0;
}