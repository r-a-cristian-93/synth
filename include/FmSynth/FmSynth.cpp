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

Instrument instruments[ninstr] = {

// amp    pitch        A           D         S           R    FM_inc     FM_as     FM_ae     FM_dec
// 0-64   0-64    ARF/ms      ARF/ms  0-0xFFFF      ARF/ms               < 256
  { 36,      0,      256,     CF(10),   0xFFFF,        512,      768,       64,       64,        0},   // Organ
  { 64,      0,      256,     CF(10),   0x8888,        512,      768,       64,       64,        0},   // Percussion_Organ
  { 64,     24,      512,    CF(600),        0,        256,      768,       23,       23,        0},   // Percussion 4"
  { 64,     12,      512,   CF(2000),        0,          4,      256,      128,       64,       64},   // E-Piano
  { 64,     12,    ARF/2,   CF(2300),        0,    ARF/100,      768,      512,      128,      128},   // Harpsicord
  { 64,     36,      512,   CF(4000),        0,          1,      200,      512,      128,       32},   // Bell
  { 64,     24,      256,    CF(100),   0xC000,          8,       64,      512,      32,         8},   // Dirty synth wah
  { 64,     12,      128,     CF(10),   0xFF00,         32,      128,      256,      128,      128},   // Bass
  { 64,      0,     8192,     CF(10),        0,         32,      400,     1024,      128,      128},   // Cymbal
  { 64,      0,      512,     CF(10),        0,         32,       96,        0,      512,      128},   // Funky
  { 64,      0,      512,     CF(10),        0,         32,      528,     1024,      768,      128},   // Vibraphone
  { 64,      0,       64,     CF(10),        0,         32,      244,     1024,      512,      128},   // Metal
  { 64,     12,      128,     CF(10),   0xFF00,         32,      256,      256,      128,      128},   // Violin

};

//  { 64,      12,     4096,      8,        0,      64,      768,      128,       64,       64},   // Pluck
//  { 64,     12,      256,     CF(10),   0xC000,         64,      160,      256,      128,       64}    // Harmonica
//  { 64,     24,      256,     CF(10),   0xC000,         64,       64,      384,      256,       64},   // Synth wah



Instrument *currentInstrument = &instruments[0];

int8_t sineTable[TABLE_SIZE] = {0};
uint16_t phaseIncrement[128] = {0};
uint16_t FMinc[nch] = {0};
uint32_t frequency[128] = {0};

uint8_t amp[nch] = {0};
uint16_t phase[nch] = {0};
uint16_t FMphase[nch] = {0};
uint32_t FMamp[nch] = {0};

uint8_t iADSR[nch] = {0};
float envADSR[nch] = {0};
int32_t FMda = 0;
uint32_t FMexp[nch] = {0};
uint32_t FMval[nch] = {0};

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

void init_instrument()
{
	FMda = currentInstrument->FM_ampl_start - currentInstrument->FM_ampl_end;

	for (int ich = 0; ich < 61; ich++)
	{
		FMinc[ich] = ((long)phaseIncrement[ich + currentInstrument->pitch_shift] * currentInstrument->FM_inc) / TABLE_SIZE;
	}
}

void fm_synth_set_instrument(uint8_t index)
{
	currentInstrument = &instruments[index];

	init_instrument();
}

void fm_synth_init()
{
	generate_sineTable();
	generate_phaseIncrement();

	fm_synth_set_instrument(6);
}
