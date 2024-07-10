#ifndef FM_SYNTH_H
#define FM_SYNTH_H

#include <cstdint>
#include <iostream>
#include <array>

// Arduino polyphonic FM sound
// * 31250 Hz sampling rate
// * 9-bit resolution
// * 4-fold polyphony (4 different tones can play simulatenously)
// * FM-synthesis with time-varying modulation amplitude
// * ADSR envelopes
// * 12 preset instruments
// Through PWM with timer1, sound is generated on pin 9
// instrument-select button on A5
// 18 sound keys on the remaining i/o pins
// by Arduino polyphonic FM sound Rolf Oldeman Feb 2019
// Licence CC BY-NC-SA 2.5 https://creativecommons.org/licenses/by-nc-sa/2.5/

#define nokey 255
#define nkeys (61)
#define MIDI_CHANNELS (16)

#define MANUAL_KEYS (61)
#define MANUAL_KEY_FIRST (36)
#define MANUAL_KEY_LAST (MANUAL_KEY_FIRST + MANUAL_KEYS)

#ifndef TABLE_SIZE
#define TABLE_SIZE (256)
#endif

#ifndef SAMPLE_RATE
#define SAMPLE_RATE (44100.0)
#endif

#define ADSR_STEP_IDLE 0
#define ADSR_STEP_ATACK 1
#define ADSR_STEP_DECAY 2
#define ADSR_STEP_SUSTAIN 3
#define ADSR_STEP_RELEASE 4

// ADSR_RATE_FACTOR
#define ARF (0xFFFF / SAMPLE_RATE * 1000)

struct Instrument
{
	uint32_t amplitude;
	uint32_t pitch_shift;
	float ADSR_a;
	float ADSR_d;
	uint32_t ADSR_s;
	float ADSR_r;
	uint32_t FM_inc;
	int32_t FM_ampl_start;
	int32_t FM_ampl_end;
	uint32_t FM_dec;
};

struct InstrumentComputedProperites {
	int32_t FMda = 0;
	uint16_t FMinc[nkeys] = {0};
};


struct Note
{
	uint16_t phase = 0;
	uint16_t FMphase = 0;

	uint32_t FMamp = 0;

	uint8_t iADSR = ADSR_STEP_IDLE;
	float envADSR = 0;

	uint8_t amp = 0;

	// Instrument properties
	uint16_t phaseIncrement = 0;

	uint32_t insAmp = 0;
	uint32_t insPitchShift = 0;
	float insADSR_a = 0;
	float insADSR_d = 0;
	uint32_t insADSR_s = 0;
	float insADSR_r = 0;
	// uint32_t insFM_inc = 0;
	int32_t insFM_ampl_start = 0;
	int32_t insFM_ampl_end = 0;
	uint32_t insFM_dec = 0;
	int32_t insFMda = 0;
	uint16_t insFMinc = 0;
};

extern Instrument instruments[MIDI_CHANNELS];
extern Note notes[nkeys * MIDI_CHANNELS];

extern int8_t sineTable[TABLE_SIZE];
extern uint16_t phaseIncrement[128];

int32_t fm_synth_generate_sample();

__attribute__((always_inline)) inline int32_t fm_synth_generate_sample()
{
	int32_t sample = 0;

	for (uint16_t noteIndex = 0; noteIndex < (nkeys * MIDI_CHANNELS); noteIndex++)
	{
		if (notes[noteIndex].iADSR == ADSR_STEP_IDLE)
			continue;

		Note& note = notes[noteIndex];

		// use only first 8 bits of the phase increment
		sample += sineTable[(note.phase + sineTable[note.FMphase >> 8] * (note.FMamp >> 16)) >> 8] * note.amp;
		// sample += sineTable[note.phase >> 8] * 1024;


		note.FMphase += note.insFMinc;
		note.phase += note.phaseIncrement;

		switch(note.iADSR)
		{
			case ADSR_STEP_ATACK:
			{
				note.envADSR += note.insADSR_a;

				if (note.envADSR >= 0xFFFF)
				{
					note.envADSR = 0xFFFF;
					note.iADSR = ADSR_STEP_DECAY;
				}
			}
			break;
			case ADSR_STEP_DECAY:
			{
				note.envADSR *= note.insADSR_d;

				if (note.envADSR <= note.insADSR_s)
				{
					note.envADSR = note.insADSR_s;
					note.iADSR = ADSR_STEP_SUSTAIN;
				}
			}
			break;
			case ADSR_STEP_RELEASE:
			{
				if (note.envADSR <= note.insADSR_r)
				{
					note.envADSR = 0;
					note.iADSR = ADSR_STEP_IDLE;
				}
				else
					note.envADSR -= note.insADSR_r;
			}
			break;
		}

		note.amp = (note.insAmp * ((uint16_t)note.envADSR >> 8)) >> 8;

		if (note.insFMda > 0) {
			if (note.FMamp > note.insFM_ampl_end)
				note.FMamp -= note.insFM_dec;
		}
		else {
			if (note.FMamp < note.insFM_ampl_end)
				note.FMamp += note.insFM_dec;
		}
	}


	return sample;
}

void fm_synth_init();

__attribute__((always_inline)) inline void fm_synth_note_on(uint8_t midiNote, uint8_t midiChannel)
{
	if (midiNote < MANUAL_KEY_FIRST || midiNote >= MANUAL_KEY_LAST)
		return;

	const uint8_t key = midiNote - MANUAL_KEY_FIRST;
	const uint8_t insChannel = midiChannel - 1;
	const uint16_t noteIndex = insChannel * nkeys + key;
	Note& note = notes[noteIndex];

	note.iADSR = ADSR_STEP_ATACK;
	note.envADSR = 0;
	note.amp = 0;
	note.FMamp = notes[noteIndex].insFM_ampl_start;
	note.phase = 0;
	note.FMphase = 0;
}

__attribute__((always_inline)) inline void fm_synth_note_off(uint8_t midiNote, uint8_t midiChannel)
{
	if (midiNote < MANUAL_KEY_FIRST || midiNote >= MANUAL_KEY_LAST)
		return;

	const uint8_t key = midiNote - MANUAL_KEY_FIRST;
	const uint8_t insChannel = midiChannel - 1;
	const uint16_t noteIndex = insChannel * nkeys + key;

	notes[noteIndex].iADSR = ADSR_STEP_RELEASE;
}

#endif