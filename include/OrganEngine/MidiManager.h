#ifndef MIDI_MANAGER_H
#define MIDI_MANAGER_H

#include <cstdint>

#define MIDI_CC_VIBRATO_FAST 111
#define MIDI_CC_VIBRATO_SLOW 112
#define MIDI_CC_VIBRATO_OFF 113
#define MIDI_CC_BANK_MSB 0
#define MIDI_CC_BANK_LSB 32

bool is_drawbar_controller(uint8_t controller);

unsigned int get_drawbar_id(uint8_t controller);

#endif