#include <FmSynth/FmSynth.h>
#include <cstdint>
#include <math.h>
#include <iostream>

Instrument instruments[ninstr] = {
// amp   pitch         A        D         S        R    FM_inc     FM_as     FM_ae     FM_dec
  { 36,      0,      256,       0,   0xFFFF,     512,      768,       64,       64,        0},   // Organ
  { 64,      0,      256,      32,   0x8888,     512,      768,       64,       64,        0},   // Percussion_Organ
  { 64,      0,     4096,       2,        0,      64,      256,      128,       64,       64},   // E-Piano
  { 64,      0,     8192,      16,        0,      32,      768,      512,      128,      128},   // Guitar
  { 64,      0,     8192,      16,        0,      32,      400,     1024,      128,      128},   // Cymbal
  { 64,      0,     4096,       8,        0,      16,      200,      512,      128,       32},   // Bell
  { 64,      0,      512,      16,        0,      32,       96,        0,      512,      128},   // Funky
  { 64,      0,      512,      16,        0,      32,      528,     1024,      768,      128},   // Vibraphone
  { 64,      0,       64,       8,        0,      32,      244,     1024,      512,      128},   // Metal
  { 64,     12,      128,      16,   0xFF00,      32,      256,      256,      128,      128},   // Violin
  { 64,      0,      128,      16,   0xFF00,      32,      128,      256,      128,      128},   // Bass
  { 64,      0,      256,      64,   0xC000,      64,       64,      384,      256,       64},   // Trumpet
  { 64,     12,      256,      32,   0xC000,      64,      160,      256,      128,       64}    // Harmonica
};

Instrument* currentInstrument = &instruments[0];



char sineTable[LUT_SIZE] = {0};
float phaseIncrement[nch] = {0};
float FMinc[nch]  = {0};
float frequency[nch] = {0};

void generate_sineTable()
{
    for (int i = 0; i < LUT_SIZE; i++)
    {
        sineTable[i] = (sin(2 * 3.14159265 * (i + 0.5) / LUT_SIZE)) * 128;
    }
}

void generate_phaseIncrement()
{
    for (int i = 0; i < 61; i++)
    {
        frequency[i] = 440.0 * pow(2.0, ( (i-21) / 12.0));
        phaseIncrement[i] = frequency[i] * (LUT_SIZE / 44100.0);
    }
}

void set_fm_increment() {
    for (int i = 0; i < 61; i++)
    {
        FMinc[i] = phaseIncrement[i] * ((float) currentInstrument->FM_inc / LUT_SIZE);
    }
}


void fm_synth_set_instrument(uint8_t index) {
    currentInstrument = &instruments[index];

    set_fm_increment();
}

void fm_synth_init() {
    generate_sineTable();
    generate_phaseIncrement();

    fm_synth_set_instrument(1);
}

float phase[nch]  = {0};
uint8_t amp[nch]    = {0};
float FMphase[nch]= {0};
unsigned int FMamp[nch]  = {0};


uint8_t         iADSR[nch]     = {0};
unsigned int envADSR[nch]   = {0};
unsigned int FMa0[nch]      = {0};
int          FMda[nch]      = {0};
unsigned int FMexp[nch]     = {0};
unsigned int FMval[nch]     = {0};

