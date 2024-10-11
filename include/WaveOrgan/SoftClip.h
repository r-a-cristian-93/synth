#ifndef SOFT_CLIP_H
#define SOFT_CLIP_H

#include <cstdint>
#include <math.h>

struct SoftClip {
    int32_t* table = nullptr;
    int32_t tableSize = 0;
    int32_t threshold = 0;
};

void SoftClip_Init(SoftClip* softClip, int32_t maxValue, int32_t minValue, int32_t ratio);

int32_t SoftClip_ProcessSample(SoftClip* softClip, int32_t sample);

#endif