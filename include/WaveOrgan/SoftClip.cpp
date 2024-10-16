#ifndef SOFTCLIP_H
#define SOFTCLIP_H

#include <cstdint>
#include <math.h>
#include <WaveOrgan/SoftClip.h>

void SoftClip_Init(SoftClip* softClip, int32_t minValue, int32_t ratio)
{

    float inputAccumulator = 0;
    float inputIncrement = M_PI / (MAX_INPUT_VALUE - minValue) / 2;

    for (uint32_t tableIndex = 0; tableIndex < minValue; tableIndex++) {
        softClip->table[tableIndex] = tableIndex;
    }

    for (uint32_t tableIndex = minValue; tableIndex <= MAX_INPUT_VALUE ; tableIndex++) {
        softClip->table[tableIndex] = minValue + (int32_t)( tanh(inputAccumulator) * (MAX_INPUT_VALUE - minValue) / ratio);

        inputAccumulator += inputIncrement;
    }
}

int32_t SoftClip_ProcessSample(SoftClip* softClip, int32_t sample)
{
    if (sample < -MAX_INPUT_VALUE)
        return -softClip->table[MAX_INPUT_VALUE];
    if (sample > MAX_INPUT_VALUE)
        return softClip->table[MAX_INPUT_VALUE];

    if (sample < 0)
        return -softClip->table[-sample];

    return softClip->table[sample];

}

#endif
