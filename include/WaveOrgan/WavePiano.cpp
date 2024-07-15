#include <WaveOrgan/WavePiano.h>

float wav_piano_phase[MIDI_NOTES_COUNT] = {0};
float wav_piano_phaseIncrement[MIDI_NOTES_COUNT] = {0};
Envelope wav_piano_notes[MIDI_NOTES_COUNT] = {};
