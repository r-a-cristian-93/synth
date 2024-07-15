#ifndef WAVE_PIANO_H
#define WAVE_PIANO_H

#ifndef MIDI_NOTES_COUNT
#define MIDI_NOTES_COUNT (128)
#endif

#ifndef LUT_SIZE
#define LUT_SIZE (1024)
#endif

#include <cstdint>
#include <WaveOrgan/Envelope.h>
#include <WaveOrgan/WaveOrgan.h>

// extern int16_t pianovoice_lut[32][LUT_SIZE];

// extern int16_t* wav_piano_active_voice;
extern float wav_piano_phase[MIDI_NOTES_COUNT];
extern float wav_piano_phaseIncrement[MIDI_NOTES_COUNT];
extern Envelope wav_piano_notes[MIDI_NOTES_COUNT];

__attribute__((always_inline)) inline int32_t wave_piano_generate_sample()
{
	int32_t sample = 0;

	for (uint16_t noteIndex = 0; noteIndex < MIDI_NOTES_COUNT; noteIndex++)
	{
		if (wav_piano_notes[noteIndex].state == ADSR_IDLE)
			continue;

		float envelopeAmpl = envelope_get_amplitude(&wav_piano_notes[noteIndex]);

        sample += (int32_t)((float)wav_active_voice[(uint16_t) wav_piano_phase[noteIndex]] * envelopeAmpl);
		// sample += wav_active_voice[(uint16_t) wav_piano_phase[noteIndex]];



		wav_piano_phase[noteIndex] += wav_phaseIncrement[noteIndex];

        while (wav_piano_phase[noteIndex] >= LUT_SIZE)
        {
            wav_piano_phase[noteIndex] -= LUT_SIZE;
        }

        while (wav_piano_phase[noteIndex] < 0)
        {
            wav_piano_phase[noteIndex] += LUT_SIZE;
        }
    }

    // >> 4 Make room for more notes
	return sample >> 2;
}

__attribute__((always_inline)) inline
void wav_piano_note_on(uint8_t midiNote) {
    wav_piano_notes[midiNote -36].state = ADSR_ATTACK;
}

__attribute__((always_inline)) inline
void wav_piano_note_off(uint8_t midiNote) {
    wav_piano_notes[midiNote-36].state = ADSR_RELEASE;
}

#endif