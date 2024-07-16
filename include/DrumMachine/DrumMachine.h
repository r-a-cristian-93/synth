#ifndef DRUM_MACHINE_H
#define DRUM_MACHINE_H

#include <cstdint>

#define INSTRUMENTS_COUNT (4)

struct DrumInstrument {
    const int16_t* lut;
    uint16_t samplesCount;
    uint16_t phase;
    bool playing;
    uint8_t velocity;
};

extern DrumInstrument instrument[INSTRUMENTS_COUNT];

__attribute__((always_inline)) inline int32_t drum_machine_generate_sample()
{
	int32_t sample = 0;

	for (uint16_t instrIndex = 0; instrIndex < INSTRUMENTS_COUNT; instrIndex++)
	{
        DrumInstrument& ins = instrument[instrIndex];

		if (ins.playing == false)
			continue;

        // 0x7FFF * 7F >> 7 = 0x7EFF
        sample += (ins.lut[ins.phase] * ins.velocity) >> 7;

		ins.phase++;

        if (ins.phase >= ins.samplesCount)
            ins.playing = false;
    }

	return sample >> 5;
}

__attribute__((always_inline)) inline
void drum_machine_play(uint8_t instrumentIndex, uint8_t velocity) {
    if (instrumentIndex >= INSTRUMENTS_COUNT)
        return;

    instrument[instrumentIndex].phase = 0;
    instrument[instrumentIndex].velocity =  velocity;
    instrument[instrumentIndex].playing = true;
}

#endif