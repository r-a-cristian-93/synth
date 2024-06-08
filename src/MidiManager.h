#ifndef MIDI_MANAGER_H
#define MIDI_MANAGER_H

#include <map>
#include <cstdint>

std::map<uint8_t, unsigned int> midi_cc_drawbar_id_map = {
    {102, 0},
    {103, 1},
    {104, 2},
    {105, 3},
    {106, 4},
    {107, 5},
    {108, 6},
    {109, 7},
    {110, 8}};

#define MIDI_CC_VIBRATO_FAST 111
#define MIDI_CC_VIBRATO_SLOW 112
#define MIDI_CC_VIBRATO_OFF 113

bool is_drawbar_controller(uint8_t controller)
{
    if (midi_cc_drawbar_id_map.find(controller) != midi_cc_drawbar_id_map.end())
        return true;
    else
        return false;
}

unsigned int get_drawbar_id(uint8_t controller)
{
    return midi_cc_drawbar_id_map.find(controller)->second;
}

#endif