#include <FmSynth/FmSynth.h>
#include <cstdint>
#include <math.h>
#include <iostream>

/*

Decay is exponential

https://www.musicdsp.org/en/latest/Synthesis/189-fast-exponential-envelope-generator.html

coeff = 1.0f + (log(levelEnd) - log(levelBegin)) / (decayTime / timeScale * sampleRate + 1);

i found timeScale = 250 to be fine for time in ms

*/

#define CF(timeMs) (1.0f + (log(0.01) - log(0xFFFF)) / (timeMs / 250.0 * SAMPLE_RATE))

Instrument instruments[MIDI_CHANNELS] = {

	// amp    pitch        A           D         S           R    FM_inc     FM_as     FM_ae     FM_dec
	// 0-64   0-64    ARF/ms      ARF/ms  0-0xFFFF      ARF/ms               < 256

	// Orchestra
	{ 16,      0,      256,     CF(10),   0xFFFF,        512,      768,     0x2CFFFF,   0x2CFFFF,        0},  //   1 - Organ
	{ 16,     12,      256,     CF(10),   0xFFFF,        512,      768,     0x2CFFFF,   0x2CFFFF,        0},  //   2 - Organ
	{ 16,     19,      256,     CF(10),   0xFFFF,        512,      768,     0x2CFFFF,   0x2CFFFF,        0},  //   3 - Organ
	{ 16,     24,      256,     CF(10),   0xFFFF,        512,      768,     0x2CFFFF,   0x2CFFFF,        0},  //   4 - Organ
	{ 16,     31,      256,     CF(10),   0xFFFF,        512,      768,     0x2CFFFF,   0x2CFFFF,        0},  //   5 - Organ

	// Effects
	{ 64,     24,      512,    CF(300),        0,        256,      768,           23,         23,        0},  //   6 - Percussion 4"
	{ 64,     12,      512,   CF(2000),        0,          4,      256,     0x7FFFFF,   0x3FFFFF,     0xFF},  //   7 - E-Piano
	{ 64,     12,    ARF/2,   CF(2300),        0,    ARF/100,      768,          512,        128,      128},  //   8 - Harpsicord
	{ 32,     36,      512,   CF(4000),        0,          1,      200,          512,        128,       32},  //   9 - Bell
	{ 64,     12,        2,     CF(10),   0xFF00,          2,      256,          256,        128,        1},  //   10 - Synth strings

	// Percussion/Accompaniament
	{ 64,      0,     1024,    CF(1200),       0,          4,      256,     0x7FFFFF,      0x7FF,    0x7FF},  //  11 - Kick - G3
	{ 64,      0,     1024,    CF(300),        0,          4,     6000,      0xFFFFF,   0xFFFFF0,   0xFFFF},  //  12 - Hihat
	{ 64,      0,      512,     CF(10),        0,         32,       96,            0,        512,      128},  //  13 - Funky
	{ 64,      0,      512,     CF(10),        0,         32,      528,         1024,        768,      128},  //  14 - Vibraphone
	{ 64,      0,       64,     CF(10),        0,         32,      244,         1024,        512,      128},  //  15 - Metal
	{ 64,      0,       64,     CF(10),        0,         32,      244,         1024,        512,      128},  //  16 - Metal
};

// { 64,     12,     4096,          8,        0,         64,      768,      128,      64,        64},  // Pluck
// { 64,     12,      256,     CF(10),   0xC000,         64,      160,      256,     128,        64},  // Harmonica
// { 64,     24,      256,     CF(10),   0xC000,         64,       64,      384,     256,        64},  // Synth wah
// { 64,     24,      256,    CF(100),   0xC000,          8,       64,      512,      32,         8},  //  10 - Dirty synth wah


Note notes[nkeys * MIDI_CHANNELS] = {};

int8_t sineTable[TABLE_SIZE] = {0};
uint16_t phaseIncrement[128] = {0};
uint32_t frequency[128] = {0};

void generate_sineTable()
{
	for (int i = 0; i < TABLE_SIZE; i++)
	{
		sineTable[i] = (sin(2 * 3.14159265 * (i + 0.5) / TABLE_SIZE)) * 128;
	}
}

void generate_phaseIncrement()
{
	for (int i = 0; i < 128; i++)
	{
		frequency[i] = 110.0 * pow(2.0, ((i - 21) / 12.0));
		phaseIncrement[i] = frequency[i] * 65536.0 / 44100.0 + 0.5;
	}
}

void init_notes() {
	for (uint8_t insIndex = 0; insIndex < MIDI_CHANNELS; insIndex++)
	{
		Instrument& instrument = instruments[insIndex];

		for (uint8_t keyIndex = 0; keyIndex < nkeys; keyIndex++)
		{
			const uint16_t noteIndex = insIndex * nkeys + keyIndex;
			Note& note = notes[noteIndex];

			note.phaseIncrement = phaseIncrement[keyIndex + instruments[insIndex].pitch_shift];
			note.insAmp = instrument.amplitude;
			note.insPitchShift = instrument.pitch_shift;
			note.insADSR_a = instrument.ADSR_a;
			note.insADSR_d = instrument.ADSR_d;
			note.insADSR_s = instrument.ADSR_s;
			note.insADSR_r = instrument.ADSR_r;
			note.insFM_ampl_start = instrument.FM_ampl_start;
			note.insFM_ampl_end = instrument.FM_ampl_end;
			note.insFM_dec = instrument.FM_dec;
			note.insFMda = instrument.FM_ampl_start - instrument.FM_ampl_end;
			note.insFMinc =
				(
					(long)phaseIncrement[keyIndex + instrument.pitch_shift] *
					instrument.FM_inc
				)
				 / TABLE_SIZE;
		}
	}
}

void fm_synth_init()
{
	generate_sineTable();
	generate_phaseIncrement();

	init_notes();
}

void fm_synth_increment_fmInc(uint8_t midiChannel) {
	const uint8_t insChannel = midiChannel - 1;

	instruments[insChannel].FM_inc += TABLE_SIZE;
	std::cout << "FM_inc: "  << instruments[insChannel].FM_inc << std::endl;

	init_notes();
}

void fm_synth_decrement_fmInc(uint8_t midiChannel) {
	const uint8_t insChannel = midiChannel - 1;

	instruments[insChannel].FM_inc -= TABLE_SIZE;
	std::cout << "FM_inc: "  << instruments[insChannel].FM_inc << std::endl;

	init_notes();
}

void fm_synth_increment_fmAsAe(uint8_t midiChannel) {
	const uint8_t insChannel = midiChannel - 1;

	instruments[insChannel].FM_ampl_start += 65536;
	instruments[insChannel].FM_ampl_end += 65536;

	std::cout << "FM_as: "  << instruments[insChannel].FM_ampl_start << std::endl;
	std::cout << "FM_ae: "  << instruments[insChannel].FM_ampl_end << std::endl;

	init_notes();
}

void fm_synth_decrement_fmAsAe(uint8_t midiChannel) {
	const uint8_t insChannel = midiChannel - 1;

	instruments[insChannel].FM_ampl_start -= 65536;
	instruments[insChannel].FM_ampl_end -= 65536;

	std::cout << "FM_as: "  << instruments[insChannel].FM_ampl_start << std::endl;
	std::cout << "FM_ae: "  << instruments[insChannel].FM_ampl_end << std::endl;

	init_notes();
}

// void fm_synth_increment_fmAs(uint8_t midiChannel);
// void fm_synth_increment_fmAe(uint8_t midiChannel);
// void fm_synth_increment_fmDec(uint8_t midiChannel);

// void fm_synth_decrement_fmInc(uint8_t midiChannel);
// void fm_synth_decrement_fmAs(uint8_t midiChannel);
// void fm_synth_decrement_fmAe(uint8_t midiChannel);
// void fm_synth_decrement_fmDec(uint8_t midiChannel);
