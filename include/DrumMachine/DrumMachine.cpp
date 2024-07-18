#include <DrumMachine/DrumMachine.h>
#include <DrumMachine/perc_kick.h>
#include <DrumMachine/perc_kick_hard.h>
#include <DrumMachine/perc_shaker.h>
#include <DrumMachine/perc_snare.h>
#include <DrumMachine/perc_stick.h>

DrumInstrument instrument[INSTRUMENTS_COUNT] = {
    {wav_perc_kick, wav_perc_kick_length,             0, false, 0},
    {wav_perc_kick_hard, wav_perc_kick_hard_length,   0, false, 0},
    {wav_perc_shaker, wav_perc_shaker_length,         0, false, 0},
    {wav_perc_snare, wav_perc_snare_length,           0, false, 0},
    {wav_perc_stick, wav_perc_stick_length,           0, false, 0}
};