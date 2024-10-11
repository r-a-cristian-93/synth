#ifndef SOFTCLIP_H
#define SOFTCLIP_H

#include <cstdint>
#include <math.h>
#include <WaveOrgan/SoftClip.h>

void SoftClip_Init(SoftClip* softClip, int32_t maxValue, int32_t minValue, int32_t ratio)
{
    softClip->table = (int32_t*) calloc(maxValue, sizeof(int32_t));
    softClip->tableSize = maxValue ;
    softClip->threshold = minValue;

    float inputAccumulator = 0;
    float inputIncrement = M_PI / (maxValue - minValue) / 2;

    for (uint32_t tableIndex = 0; tableIndex < minValue; tableIndex++) {
        softClip->table[tableIndex] = tableIndex;
    }

    for (uint32_t tableIndex = minValue; tableIndex < maxValue; tableIndex++) {
        softClip->table[tableIndex] = minValue + (int32_t)( tanh(inputAccumulator) * (maxValue - minValue) / ratio);

        inputAccumulator += inputIncrement;
    }
}

int32_t SoftClip_ProcessSample(SoftClip* softClip, int32_t sample)
{
    if (sample < -softClip->tableSize)
        return -softClip->table[softClip->tableSize - 1];
    if (sample > softClip->tableSize)
        return softClip->table[softClip->tableSize - 1];

    if (sample < 0)
        return -softClip->table[-sample];

    return softClip->table[sample];

}

#endif