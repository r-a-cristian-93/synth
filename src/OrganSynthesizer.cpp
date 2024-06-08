#define MINIAUDIO_IMPLEMENTATION

#include "OrganSynthesizer.h"

#include <cmath>
#include <list>
#include <mutex>

#include <RtMidi/RtMidi.h>
#include <nanomidi/decoder.h>
#include <nanomidi/msgprint.h>
#include <miniaudio/miniaudio.h>

#include "SharedResources.h"
#include "Config.h"
#include "Parameter.h"
#include "Envelope.h"
#include "MidiManager.h"
#include "Note.h"


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