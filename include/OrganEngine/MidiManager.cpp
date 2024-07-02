#include "MidiManager.h"

#include <map>

std::map<uint8_t, unsigned int> midi_cc_drawbar_id_map = {
    {70, 0},
    {71, 1},
    {72, 2},
    {73, 3},
    {74, 4},
    {75, 5},
    {76, 6},
    {77, 7},
    {78, 8}};

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
