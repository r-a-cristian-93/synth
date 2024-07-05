#ifndef FM_SYNTH_H
#define FM_SYNTH_H

#include <cstdint>

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

#ifndef LUT_SIZE
#define LUT_SIZE (1024)
#endif


#define ADSR_STEP_NONE 0
#define ADSR_STEP_ATACK 1
#define ADSR_STEP_DECAY 2
#define ADSR_STEP_SUSTAIN 3
#define ADSR_STEP_RELEASE 4

struct Instrument
{
  unsigned int amplitude;
  unsigned int pitch;
  unsigned int ADSR_a;
  unsigned int ADSR_d;
  unsigned int ADSR_s;
  unsigned int ADSR_r;
  unsigned int FM_inc;
  unsigned int FM_ampl_start;
  unsigned int FM_ampl_end;
  unsigned int FM_dec;
};

extern Instrument instruments[ninstr];
extern char sineTable[LUT_SIZE];
extern float phaseIncrement[61];


//initialize the main parameters of the pulse length setting
extern float phase[nch];
extern float          inc[nch];
extern uint8_t      amp[nch];
extern float FMphase[nch];
extern float FMinc[nch];
extern unsigned int FMamp[nch];

// main function (forced inline) to update the pulse length
int fm_synth_generate_sample();
void updateParameters();

__attribute__((always_inline)) inline
int fm_synth_generate_sample() {
    int sample = 0;

    for (uint8_t ich = 0; ich < nch; ich++)
    {
        // sample += sineTable[((int)phase[ich]+sineTable[(int)FMphase[ich]>>8]*FMamp[ich]) >> 8] * amp[ich];
        int phase_shift = (int)(sineTable[(int)FMphase[ich]] * ((float) FMamp[ich]) / 32);
        float ph = phase[ich] + phase_shift;

        while (ph >= LUT_SIZE) {
            ph -= LUT_SIZE;
        }

        while (ph < 0) {
            ph += LUT_SIZE;
        }

        sample += sineTable[(int)ph] * amp[ich];

        // simple sine - ok
        // sample += sineTable[((int)phase[ich])] * amp[ich];


        FMphase[ich] += FMinc[ich];
        phase[ich] += inc[ich];

        while (FMphase[ich] >= LUT_SIZE) {
            FMphase[ich] -= LUT_SIZE;
        }

        while (FMphase[ich] < 0) {
            FMphase[ich] += LUT_SIZE;
        }

        while (phase[ich] >= LUT_SIZE) {
            phase[ich] -= LUT_SIZE;
        }

        while (phase[ich] < 0) {
            phase[ich] += LUT_SIZE;
        }
    }

    updateParameters();

    return sample >> 2;
}

//properties of each note played
extern uint8_t         iADSR[nch];
extern unsigned int envADSR[nch];
extern unsigned int ADSRa[nch];
extern unsigned int ADSRd[nch];
extern unsigned int ADSRs[nch];
extern unsigned int ADSRr[nch];
extern uint8_t         amp_base[nch];
extern float inc_base[nch];
extern unsigned int FMa0[nch];
extern int          FMda[nch];
extern float FMinc_base[nch];
extern unsigned int FMdec[nch];
extern unsigned int FMexp[nch];
extern unsigned int FMval[nch];
extern uint8_t         keych[nch];

extern Instrument* currentInstrument;
extern uint8_t instr;

__attribute__((always_inline)) inline
void changeInstrument()
{
  currentInstrument++;

  if (currentInstrument >= &instruments[ninstr])
    currentInstrument = &instruments[0];
}

void fm_synth_init();

// main loop. Duration of loop is determined by number of setPWM calls
// Each setPWMcall corresponds to 512 cylcles=32mus
// Tloop= 32mus * #setPWM. #setPWM=9 gives Tloop=0.288ms

// inline void fineNextChanel() {
//   //find the best channel to start a new note
//   nextch = 255;

//   //then check for an empty channel
//   if (nextch == 255) {
//     if (iADSR[0] == ADSR_STEP_NONE)nextch = 0;
//     if (iADSR[1] == ADSR_STEP_NONE)nextch = 1;
//     if (iADSR[2] == ADSR_STEP_NONE)nextch = 2;
//     if (iADSR[3] == ADSR_STEP_NONE)nextch = 3;

//     //otherwise use the channel with the longest playing note
//     nextch = 0;
//     if (tch[0] > tch[nextch])nextch = 0;
//     if (tch[1] > tch[nextch])nextch = 1;
//     if (tch[2] > tch[nextch])nextch = 2;
//     if (tch[3] > tch[nextch])nextch = 3;
//   }
// }

__attribute__((always_inline)) inline
void fm_synth_note_on(uint8_t keypressed, uint8_t velocity) {
	if (keypressed < MANUAL_KEY_FIRST || keypressed >= MANUAL_KEY_LAST)
		return;

  const uint8_t nextch = keypressed - MANUAL_KEY_FIRST;

  phase[nextch]=0;
  amp_base[nextch] = currentInstrument->amplitude * velocity / 100; // / 127
  inc_base[nextch] = phaseIncrement[currentInstrument->pitch+keypressed];
  ADSRa[nextch]=currentInstrument->ADSR_a;
  ADSRd[nextch]=currentInstrument->ADSR_d;
  ADSRs[nextch]=currentInstrument->ADSR_s;
  ADSRr[nextch]=currentInstrument->ADSR_r;
  iADSR[nextch] = ADSR_STEP_ATACK;
  FMphase[nextch]=0;
  FMinc_base[nextch] = ((float)inc_base[nextch]*currentInstrument->FM_inc)/LUT_SIZE;
  FMa0[nextch] = currentInstrument->FM_ampl_end;
  FMda[nextch] = currentInstrument->FM_ampl_start-currentInstrument->FM_ampl_end;
  FMexp[nextch]=0xFFFF;
  FMdec[nextch]=currentInstrument->FM_dec;
  keych[nextch] = keypressed;
//   tch[nextch] = 0;
}

__attribute__((always_inline)) inline
void fm_synth_note_off(uint8_t keyreleased) {
  if (keyreleased < MANUAL_KEY_FIRST || keyreleased >= MANUAL_KEY_LAST)
    return;

  iADSR[keyreleased - MANUAL_KEY_FIRST] = ADSR_STEP_RELEASE;
}

__attribute__((always_inline)) inline
void updateParameters()
{
  //update FM decay exponential
  for (uint8_t ich = 0; ich < nch; ich++) {
    FMexp[ich]-=(long)FMexp[ich]*FMdec[ich]>>16;
  }

  //adjust the ADSR envelopes
  for (uint8_t ich = 0; ich < nch; ich++) {
    // RELEASE
    if (iADSR[ich] == ADSR_STEP_RELEASE) {
      if (envADSR[ich] <= ADSRr[ich]) {
        envADSR[ich] = 0;
        iADSR[ich] = ADSR_STEP_NONE;
      }
      else envADSR[ich] -= ADSRr[ich];
    }
    // DECAY
    if (iADSR[ich] == ADSR_STEP_DECAY) {
      if (envADSR[ich] <= (ADSRs[ich] + ADSRd[ich])) {
        envADSR[ich] = ADSRs[ich];
        iADSR[ich] = ADSR_STEP_SUSTAIN;
      }
      else envADSR[ich] -= ADSRd[ich];
    }
    // ATTACK
    if (iADSR[ich] == ADSR_STEP_ATACK) {
      if ((0xFFFF - envADSR[ich]) <= ADSRa[ich]) {
        envADSR[ich] = 0xFFFF;
        iADSR[ich] = ADSR_STEP_DECAY;
      }
      else envADSR[ich] += ADSRa[ich];
    }
  }

  //update the tone for channel 0
  for (uint8_t ich = 0; ich < nch; ich++) {
    amp[ich] = (amp_base[ich] * (envADSR[ich] >> 8)) >> 8;
    inc[ich] = inc_base[ich];
    FMamp[ich] = FMa0[ich] + ((long)FMda[ich] * FMexp[ich]>>16);
    FMinc[ich] = FMinc_base[ich];
  }
}

#endif