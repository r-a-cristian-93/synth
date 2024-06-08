#define MINIAUDIO_IMPLEMENTATION

#include "OrganSynthesizer.h"

#include <cmath>
#include <list>
#include <mutex>

#include <RtMidi/RtMidi.h>
#include <nanomidi/decoder.h>
#include <nanomidi/msgprint.h>
#include <miniaudio/miniaudio.h>

#include "Config.h"
#include "Parameter.h"
#include "Envelope.h"
#include "MidiManager.h"
#include "Note.h"

RtMidiIn *midiDevice = 0;

std::list<Note> notesList;
std::mutex notesMutex;

void generateSamples(ma_device* pDevice, float* pOutput, ma_uint32 frameCount)
{
    for (ma_uint32 iFrame = 0; iFrame < frameCount; iFrame++)
    {
        double sample = 0;

        osc_update();

        {
            // Get a lock on notesList
            const std::lock_guard<std::mutex> lock(notesMutex);

            for (Note &note : notesList)
            {
                sample += osc_generate_sample(note);
            }
        }

        // Limit volume so we won't blow up speakers
        if (sample > MAX_AMPLITUDE)
            sample = MAX_AMPLITUDE;
        if (sample < -MAX_AMPLITUDE)
            sample = -MAX_AMPLITUDE;

        // Add sample to left channel
        *pOutput++ = (float)sample;
        // Add sample to right channel
        *pOutput++ = (float)sample;
    }
}

void clearSilencedNotes()
{
    const std::lock_guard<std::mutex> lock(notesMutex);

    for (auto it = notesList.begin(); it != notesList.end(); it++)
    {
        // Remove one by one in the order they were added
        if (std::abs(it->envelope.state) == ADSR_IDLE)
        {
            notesList.erase(it++);
            break;
        }
    }
}

void dataCallback(ma_device *pDevice, void *pOutput, const void *pInput, ma_uint32 frameCount)
{
    generateSamples(pDevice, (float *)pOutput, frameCount);
    clearSilencedNotes();
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

        notesList.emplace_back(Note{message->data.note_on.note});
    }
    break;

    case MIDI_TYPE_NOTE_OFF:
    {
        // Call NoteOff on first occurence
        const std::lock_guard<std::mutex> lock(notesMutex);

        for (Note &note : notesList)
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

        if (controller == MIDI_CC_VIBRATO_FAST)
            osc_set_vibrato_fast();

        if (controller == MIDI_CC_VIBRATO_SLOW)
            osc_set_vibrato_slow();

        if (controller == MIDI_CC_VIBRATO_OFF)
            osc_set_vibrato_off();

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
