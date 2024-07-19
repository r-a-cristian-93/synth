#ifndef SEQUENCER_H
#define SEQUENCER_H

#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <vector>
#include <DrumMachine/DrumMachine.h>

#define TRACKS_COUNT (5)
#define SEQUENCES_COUNT (7)
#ifndef SAMPLE_RATE
#define SAMPLE_RATE (44100)
#endif

typedef std::vector<uint8_t> Sequence[TRACKS_COUNT];

extern const Sequence* sequences[];

extern uint8_t active_sequence;
extern uint8_t current_step;
extern uint32_t sample_counter;
extern uint32_t samples_per_step;


// inline
void sequencer_set_bpm(uint32_t bpm);
void sequencer_set_sequence(uint8_t sequenceNumber);

__attribute__((always_inline)) inline
void sequencer_init()
{
    active_sequence = 0;
    current_step = 0;
    sample_counter = 0;
    sequencer_set_bpm(88);
    sequencer_set_sequence(6);
}

__attribute__((always_inline)) inline
void sequencer_set_bpm(uint32_t bpm)
{
    // Assuming 16th notes
    samples_per_step = (60 * SAMPLE_RATE) / (bpm * 4);
}

__attribute__((always_inline)) inline
void sequencer_set_sequence(uint8_t sequenceNumber)
{
    if (sequenceNumber < SEQUENCES_COUNT)
    {
        active_sequence = sequenceNumber;
    }
}

__attribute__((always_inline)) inline
void sequencer_tick()
{
    sample_counter++;
    if (sample_counter >= samples_per_step)
    {
        sample_counter = 0;

        const Sequence& seq = *sequences[active_sequence];

        for (int track = 0; track < TRACKS_COUNT; ++track)
        {
            if (seq[track][current_step] > 0)
            {
                drum_machine_play(track, seq[track][current_step]);
            }
        }

        // Advance to the next step, wrapping around if necessary based on the number of steps in the sequence
        current_step = (current_step + 1) % seq[0].size();
    }
}

#endif
