#ifndef ORGAN_SYNTHESIZER_H
#define ORGAN_SYNTHESIZER_H

#include <miniaudio/miniaudio.h>
#include <OrganEngine/OrganOscillator.h>

void dataCallback(ma_device *pDevice, void *pOutput, const void *pInput, ma_uint32 frameCount);

#endif