#ifndef FM_SYNTH_H
#define FM_SYNTH_H

#include <cstdint>
#include <iostream>

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

#define ninstr 13
#define nokey 255
#define nch 61
#define MANUAL_KEYS (61)
#define MANUAL_KEY_FIRST (36)
#define MANUAL_KEY_LAST (MANUAL_KEY_FIRST + MANUAL_KEYS)

#ifndef TABLE_SIZE
#define TABLE_SIZE (256)
#endif

#ifndef SAMPLE_RATE
#define SAMPLE_RATE (44100.0)
#endif

#define ADSR_STEP_NONE 0
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

extern Instrument instruments[ninstr];

extern int8_t sineTable[TABLE_SIZE];
extern uint16_t phaseIncrement[128];
extern uint16_t FMinc[nch];

// initialize the main parameters of the pulse length setting
extern uint8_t amp[nch];
extern uint16_t phase[nch];
extern uint16_t FMphase[nch];
extern uint32_t FMamp[nch];

// properties of each note played
extern uint8_t iADSR[nch];
extern float envADSR[nch];
extern int32_t FMda;
extern uint32_t FMexp[nch];
extern uint32_t FMval[nch];

extern Instrument *currentInstrument;

int32_t fm_synth_generate_sample();
void updateParameters();

__attribute__((always_inline)) inline int32_t fm_synth_generate_sample()
{
	int32_t sample = 0;

	for (uint8_t ich = 0; ich < nch; ich++)
	{
		// use only first 8 bits of the phase increment
		sample += sineTable[(phase[ich] + sineTable[FMphase[ich] >> 8] * FMamp[ich]) >> 8] * amp[ich];

		FMphase[ich] += FMinc[ich];
		phase[ich] += phaseIncrement[ich + currentInstrument->pitch_shift];
	}

	updateParameters();

	return sample;
}

void fm_synth_init();
void init_instrument();

__attribute__((always_inline)) inline void fm_synth_next_instrument()
{
	currentInstrument++;

	if (currentInstrument >= &instruments[ninstr])
		currentInstrument = &instruments[0];

	init_instrument();
}

__attribute__((always_inline)) inline void fm_synth_note_on(uint8_t midiNote, uint8_t velocity)
{
	if (midiNote < MANUAL_KEY_FIRST || midiNote >= MANUAL_KEY_LAST)
		return;

	const uint8_t note = midiNote - MANUAL_KEY_FIRST;

	iADSR[note] = ADSR_STEP_ATACK;
	FMexp[note] = 0xFFFF;
}

__attribute__((always_inline)) inline void fm_synth_note_off(uint8_t midiNote)
{
	if (midiNote < MANUAL_KEY_FIRST || midiNote >= MANUAL_KEY_LAST)
		return;

	iADSR[midiNote - MANUAL_KEY_FIRST] = ADSR_STEP_RELEASE;
}

__attribute__((always_inline)) inline void updateParameters()
{
	// update FM decay exponential
	for (uint8_t ich = 0; ich < nch; ich++)
	{
		FMexp[ich] -= (long)FMexp[ich] * currentInstrument->FM_dec >> 16;
	}

	// adjust the ADSR envelopes
	for (uint8_t ich = 0; ich < nch; ich++)
	{
		// RELEASE
		if (iADSR[ich] == ADSR_STEP_RELEASE)
		{
			if (envADSR[ich] <= currentInstrument->ADSR_r)
			{
				envADSR[ich] = 0;
				iADSR[ich] = ADSR_STEP_NONE;
			}
			else
				envADSR[ich] -= currentInstrument->ADSR_r;
		}
		// DECAY
		if (iADSR[ich] == ADSR_STEP_DECAY)
		{
			envADSR[ich] *= currentInstrument->ADSR_d;

			if (envADSR[ich] <= currentInstrument->ADSR_s)
			{
				envADSR[ich] = currentInstrument->ADSR_s;
				iADSR[ich] = ADSR_STEP_SUSTAIN;
			}
		}
		// ATTACK
		if (iADSR[ich] == ADSR_STEP_ATACK)
		{
			envADSR[ich] += currentInstrument->ADSR_a;

			if (envADSR[ich] >= 0xFFFF)
			{
				envADSR[ich] = 0xFFFF;
				iADSR[ich] = ADSR_STEP_DECAY;
			}
		}
	}

	// update the tone for channel 0
	for (uint8_t ich = 0; ich < nch; ich++)
	{
		amp[ich] = (currentInstrument->amplitude * ((uint16_t)envADSR[ich] >> 8)) >> 8;
		FMamp[ich] = currentInstrument->FM_ampl_end + ((long)FMda * FMexp[ich] >> 16);
	}
}

#endif