
#include <WaveOrgan/WaveOrgan.h>

float wav_phase[MIDI_NOTES_COUNT] = {0};
float wav_phaseIncrement[MIDI_NOTES_COUNT] = {0};
float wav_frequency[MIDI_NOTES_COUNT] = {0};
bool wav_notes[MIDI_NOTES_COUNT] = {false};