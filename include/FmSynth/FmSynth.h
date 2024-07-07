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
	uint32_t FM_ampl_start;
	uint32_t FM_ampl_end;
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
	uint32_t FMexp = 0;
	uint32_t FMval = 0;

	uint8_t iADSR = ADSR_STEP_IDLE;
	float envADSR = 0;

	uint8_t amp = 0;
};

extern Instrument instruments[MIDI_CHANNELS];
extern InstrumentComputedProperites instrComp[MIDI_CHANNELS];
extern Note notes[MIDI_CHANNELS][nkeys];

extern int8_t sineTable[TABLE_SIZE];
extern uint16_t phaseIncrement[128];


int32_t fm_synth_generate_sample();
void updateParameters(uint8_t instrIndex, uint8_t keyIndex);

__attribute__((always_inline)) inline int32_t fm_synth_generate_sample()
{
	int32_t sample = 0;

	for (uint8_t iInstr = 0; iInstr < MIDI_CHANNELS; iInstr++)
	{
		for (uint8_t iKey = 0; iKey < nkeys; iKey++)
		{
			if (notes[iInstr][iKey].iADSR == ADSR_STEP_IDLE)
				continue;

			Note& note = notes[iInstr][iKey];

			// use only first 8 bits of the phase increment
			sample += sineTable[(note.phase + sineTable[note.FMphase >> 8] * note.FMamp) >> 8] * note.amp;

			note.FMphase += instrComp[iInstr].FMinc[iKey];
			note.phase += phaseIncrement[iKey + instruments[iInstr].pitch_shift];

			updateParameters(iInstr, iKey);
		}
	}

	return sample;
}

void fm_synth_init();

__attribute__((always_inline)) inline void fm_synth_note_on(uint8_t midiNote, uint8_t midiChannel)
{
	if (midiNote < MANUAL_KEY_FIRST || midiNote >= MANUAL_KEY_LAST)
		return;

	const uint8_t note = midiNote - MANUAL_KEY_FIRST;

	notes[midiChannel - 1][note].iADSR = ADSR_STEP_ATACK;
	notes[midiChannel - 1][note].FMexp = 0xFFFF;
}

__attribute__((always_inline)) inline void fm_synth_note_off(uint8_t midiNote, uint8_t midiChannel)
{
	if (midiNote < MANUAL_KEY_FIRST || midiNote >= MANUAL_KEY_LAST)
		return;

	const uint8_t note = midiNote - MANUAL_KEY_FIRST;

	notes[midiChannel - 1][note].iADSR = ADSR_STEP_RELEASE;
}

__attribute__((always_inline)) inline void updateParameters(uint8_t instrIndex, uint8_t keyIndex)
{
	Note& note = notes[instrIndex][keyIndex];
	Instrument& instrument = instruments[instrIndex];

	note.FMexp -= (long)note.FMexp * instrument.FM_dec >> 16;

	// RELEASE
	if (note.iADSR == ADSR_STEP_RELEASE)
	{
		if (note.envADSR <= instrument.ADSR_r)
		{
			note.envADSR = 0;
			note.iADSR = ADSR_STEP_IDLE;
		}
		else
			note.envADSR -= instrument.ADSR_r;
	}
	// DECAY
	if (note.iADSR == ADSR_STEP_DECAY)
	{
		note.envADSR *= instrument.ADSR_d;

		if (note.envADSR <= instrument.ADSR_s)
		{
			note.envADSR = instrument.ADSR_s;
			note.iADSR = ADSR_STEP_SUSTAIN;
		}
	}
	// ATTACK
	if (note.iADSR == ADSR_STEP_ATACK)
	{
		note.envADSR += instrument.ADSR_a;

		if (note.envADSR >= 0xFFFF)
		{
			note.envADSR = 0xFFFF;
			note.iADSR = ADSR_STEP_DECAY;
		}
	}

	note.amp = (instrument.amplitude * ((uint16_t)note.envADSR >> 8)) >> 8;
	note.FMamp = instrument.FM_ampl_end + ((long)instrComp[instrIndex].FMda * note.FMexp >> 16);
}

#endif