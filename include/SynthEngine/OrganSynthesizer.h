#ifndef ORGAN_SYNTHESIZER_H
#define ORGAN_SYNTHESIZER_H

#include <vector>
#include <iostream>

#include <miniaudio/miniaudio.h>

#include <SynthEngine/OrganOscillator.h>

void dataCallback(ma_device *pDevice, void *pOutput, const void *pInput, ma_uint32 frameCount);


#endif