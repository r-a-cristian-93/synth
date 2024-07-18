#include <DrumMachine/Sequencer.h>


const Sequence waltz ={};
const Sequence tango = {};
const Sequence march = {};
const Sequence swing = {};
const Sequence slow_rock = {};

const Sequence disco = {
    {127,    0,    0,    0,    0,    0,    0,    0,  127,    0,    0,    0,    0,    0,    0,    0},
    {  0,    0,    0,    0,  127,    0,    0,    0,    0,    0,    0,    0,  127,    0,    0,    0},
    {  0,    0,  127,    0,    0,    0,  127,    0,    0,    0,  127,    0,    0,    0,  127,    0},
    {  0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0},
    {  0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0}
};

const Sequence samba = {};
const Sequence bossa_nova = {};

const Sequence* sequences[] = {
    &waltz,
    &tango,
    &march,
    &swing,
    &slow_rock,
    &disco,
    &samba,
    &bossa_nova
};

uint8_t active_sequence = 0;
uint8_t current_step = 0;
uint32_t sample_counter = 0;
uint32_t samples_per_step = 0;
