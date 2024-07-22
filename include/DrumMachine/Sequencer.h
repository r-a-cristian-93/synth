#ifndef SEQUENCER_H
#define SEQUENCER_H

#include <cstdint>
#include <DrumMachine/DrumMachine.h>

#define TRACKS_COUNT (5)
#define SEQUENCES_COUNT (8)
#ifndef SAMPLE_RATE
#define SAMPLE_RATE (44100)
#endif

extern const uint8_t* sequences[];
extern const uint8_t sequences_length[];
extern uint32_t sample_counter;
extern uint32_t samples_per_step;

extern const uint8_t* first_trigger;
extern const uint8_t* last_trigger;
extern const uint8_t* current_trigger;


// inline
void sequencer_set_bpm(uint32_t bpm);
void sequencer_set_sequence(uint8_t sequenceNumber);

__attribute__((always_inline)) inline
void sequencer_init()
{
    sample_counter = 0;
    sequencer_set_bpm(90);
    sequencer_set_sequence(0);
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
        first_trigger = sequences[sequenceNumber];
        last_trigger = first_trigger + sequences_length[sequenceNumber] * TRACKS_COUNT;
        current_trigger = first_trigger;
    }
}

__attribute__((always_inline)) inline
void sequencer_tick()
{
    sample_counter++;

    if (sample_counter < samples_per_step)
    {
        return;
    }

    sample_counter = 0;

    for (int track = 0; track < TRACKS_COUNT; ++track)
    {
        int current_velocity = *(current_trigger++);

        if (current_velocity > 0)
        {
            drum_machine_play(track, current_velocity);
        }
    }

    // Wrapping around based on the number of steps in the sequence
    if (current_trigger == last_trigger)
    {
        current_trigger = first_trigger;
    }
}


#endif
