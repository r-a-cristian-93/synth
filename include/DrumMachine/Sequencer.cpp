#include <DrumMachine/Sequencer.h>


const uint8_t seq1_steps = 8;
const uint8_t seq1_values[TRACKS_COUNT][seq1_steps] = {
    {127,    0,  127,    0,  127,    0,  127,    0},
    {  0,  100,  100,  100,  100,  100,  100,  100},
    {  0,    0,    0,    0,    0,    0,    0,    0},
    {  0,    0,    0,    0,    0,    0,    0,    0}
};

const uint8_t seq2_steps = 8;
const uint8_t seq2_values[TRACKS_COUNT][seq2_steps] = {
    {127,    0,  127,    0,  127,    0,  127,    0},
    {  0,  100,  100,  100,  100,  100,  100,  100},
    {  0,    0,    0,    0,    0,    0,    0,    0},
    {  0,   80,    0,   80,    0,   80,   80,   80}
};

const Sequence sequences[] = {
    {seq1_steps, seq1_values},
    {seq2_steps, seq2_values}
};

uint8_t active_sequence = 0;
uint8_t current_step = 0;
uint32_t sample_counter = 0;
uint32_t samples_per_step = 0;
