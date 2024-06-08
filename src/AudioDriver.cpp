#include "AudioDriver.h"

#include <iostream>
#include <miniaudio/miniaudio.h>

#include "OrganSynthesizer.h"

ma_context audioContext;
ma_device audioDevice;

int audio_driver_init()
{
    ma_result result;

    result = ma_context_init(NULL, 0, NULL, &audioContext);

    if (result != MA_SUCCESS)
    {
        std::cerr << "Failed to initialize miniaudio context." << std::endl;
        return 1;
    }

    ma_device_config audioConfig = ma_device_config_init(ma_device_type_playback);
    audioConfig.playback.format = ma_format_f32; // Set to ma_format_unknown to use the device's native format.
    audioConfig.playback.channels = 2;           // Set to 0 to use the device's native channel count.
    audioConfig.sampleRate = 44100;              // Set to 0 to use the device's native sample rate.
    audioConfig.dataCallback = dataCallback;     // This function will be called when miniaudio needs more data.
    audioConfig.pUserData = NULL;                // Can be accessed from the device object (device.pUserData).

    result = ma_device_init(NULL, &audioConfig, &audioDevice);

    if (result != MA_SUCCESS)
    {
        std::cerr << "Failed to initialize playback device." << std::endl;
        ma_context_uninit(&audioContext);
        return 1;
    }

    result = ma_device_start(&audioDevice);

    if (result != MA_SUCCESS)
    {
        std::cerr << "Failed to start playback device." << std::endl;
        ma_device_uninit(&audioDevice);
        ma_context_uninit(&audioContext);
        return 1;
    }

    std::cout << "Internal sample rate: " << audioDevice.playback.internalSampleRate << std::endl;
    std::cout << "Playback device name: " << audioDevice.playback.name << std::endl;
}

void audio_driver_close()
{
    ma_device_uninit(&audioDevice);
    ma_context_uninit(&audioContext);
}