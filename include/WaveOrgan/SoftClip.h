#ifndef SOFT_CLIP_H
#define SOFT_CLIP_H

#include <cstdint>
#include <math.h>

#define MAX_INPUT_VALUE (65535)

struct SoftClip {
    int16_t table[MAX_INPUT_VALUE + 1] = {0};
};

void SoftClip_Init(SoftClip* softClip, int32_t threshold, int32_t ratio);

int32_t SoftClip_ProcessSample(SoftClip* softClip, int32_t sample);

#endif
