#define MINIAUDIO_IMPLEMENTATION
#include <iostream>
#include "miniaudio/miniaudio.h"
#include <cmath>
#include <conio.h>

#define HARMONICS_COUNT 9
#define HARMONICS_AMPLITUDE_MAX 1.0
#define HARMONICS_AMPLITUDE_MIN 0.0
#define HARMONICS_AMPLITUDE_INCREMENT 0.02


struct Parameter {
    double current_value;
    double target_value;

    double change_time_start;

    const double min_value;
    const double max_value;
    const double increment_value;
};




const double freq[HARMONICS_COUNT] = {32.7,65.4,98.1,130.8,163.5,196.2,228.9,261.6,294.3};

double harmonics_amplitude[HARMONICS_COUNT] = {0};
double target_harmonics_amplitude[HARMONICS_COUNT] = {0};
double change_start_time_harmonics_amplitude[HARMONICS_COUNT] = {0};


double detune = 0.0;
double phase = 0.0;

double synth_time = 0;
double parameter_change_duration = 100; //ms



void increase_harmonics_amplitude(const int harmonics_index)
{
    change_start_time_harmonics_amplitude[harmonics_index] = synth_time;

    if (target_harmonics_amplitude[harmonics_index] < HARMONICS_AMPLITUDE_MAX)
        target_harmonics_amplitude[harmonics_index] += HARMONICS_AMPLITUDE_INCREMENT;
}

void decrease_harmonics_amplitude(const int harmonics_index)
{
    change_start_time_harmonics_amplitude[harmonics_index] = synth_time;

    if (target_harmonics_amplitude[harmonics_index] > HARMONICS_AMPLITUDE_MIN)
        target_harmonics_amplitude[harmonics_index] -= HARMONICS_AMPLITUDE_INCREMENT;

    // some precision problem
    if (target_harmonics_amplitude[harmonics_index] < HARMONICS_AMPLITUDE_INCREMENT)
        target_harmonics_amplitude[harmonics_index] = HARMONICS_AMPLITUDE_MIN;
}

void update_harmonics_amplitude(const int harmonics_index)
{
    const double &target = target_harmonics_amplitude[harmonics_index];
    double &current = harmonics_amplitude[harmonics_index];

    if (target != current)
    {
        double dt = synth_time - change_start_time_harmonics_amplitude[harmonics_index];
        double step_value = pow(synth_time - change_start_time_harmonics_amplitude[harmonics_index], 2.0);

        if (target < current)
        {
            current -= step_value;

            if (current < target) current = target;
        }

        if (target > current)
        {
            current += step_value;

            if (current > target) current = target;
        }
    }
}

void play_harmonics(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount, int harmonicsCount)
{
    double g_amplitude = 0.1;
    float* pOutputF32 = (float*)pOutput;

    for (ma_uint32 iFrame = 0; iFrame < frameCount; iFrame++)
    {
        double value = 0;
        double trigger = (g_amplitude * sin(2.0 * M_PI * (freq[0] + freq[0] * detune) * synth_time));

        for (int i = 0; i < harmonicsCount; i++) {
            update_harmonics_amplitude(i);

            value += g_amplitude * harmonics_amplitude[i] * sin(2.0 * M_PI * (freq[i] + freq[i] * detune) * (synth_time + phase * i));
        }

        // left channel
        *pOutputF32++ = (float) trigger;
        // right channel
        *pOutputF32++ = (float) value;

        synth_time += 1.0 / (double) pDevice->playback.internalSampleRate;
    }
}

void DataCallback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount)
{
    play_harmonics(pDevice, pOutput, pInput, frameCount, HARMONICS_COUNT);
}

int main() {
    ma_result result;
    ma_context context;
    ma_device device;

    // Initialize miniaudio context
    result = ma_context_init(NULL, 0, NULL, &context);
    if (result != MA_SUCCESS)
    {
        std::cerr << "Failed to initialize miniaudio context." << std::endl;
        return 1;
    }

    ma_device_config config = ma_device_config_init(ma_device_type_playback);
    config.playback.format   = ma_format_f32;   // Set to ma_format_unknown to use the device's native format.
    config.playback.channels = 2;               // Set to 0 to use the device's native channel count.
    config.sampleRate        = 44100;           // Set to 0 to use the device's native sample rate.
    config.dataCallback      = DataCallback;   // This function will be called when miniaudio needs more data.
    config.pUserData         = NULL;   // Can be accessed from the device object (device.pUserData).

    result = ma_device_init(NULL, &config, &device);
    if (result != MA_SUCCESS)
    {
        std::cerr << "Failed to initialize playback device." << std::endl;
        ma_context_uninit(&context);
        return 1;
    }

    result = ma_device_start(&device);
    if (result != MA_SUCCESS)
    {
        std::cerr << "Failed to start playback device." << std::endl;
        ma_device_uninit(&device);
        ma_context_uninit(&context);
        return 1;
    }

    // Wait for user input (you can adjust this as needed)
    std::cout << "Press ESC to exit..." << std::endl;
    while (true)
    {
        int input = getch();

        //std::cout << input << std::endl;

        if (input == 27) break;

        switch (input)
        {
            case '1': increase_harmonics_amplitude(0); break;
            case '2': increase_harmonics_amplitude(1); break;
            case '3': increase_harmonics_amplitude(2); break;
            case '4': increase_harmonics_amplitude(3); break;
            case '5': increase_harmonics_amplitude(4); break;
            case '6': increase_harmonics_amplitude(5); break;
            case '7': increase_harmonics_amplitude(6); break;
            case '8': increase_harmonics_amplitude(7); break;
            case '9': increase_harmonics_amplitude(8); break;

            case 'q': decrease_harmonics_amplitude(0); break;
            case 'w': decrease_harmonics_amplitude(1); break;
            case 'e': decrease_harmonics_amplitude(2); break;
            case 'r': decrease_harmonics_amplitude(3); break;
            case 't': decrease_harmonics_amplitude(4); break;
            case 'y': decrease_harmonics_amplitude(5); break;
            case 'u': decrease_harmonics_amplitude(6); break;
            case 'i': decrease_harmonics_amplitude(7); break;
            case 'o': decrease_harmonics_amplitude(8); break;

            case 'a': phase += 0.00001; break;
            case 'z': phase -= 0.00001; break;

            case 's': detune += 0.01; break;
            case 'x': detune -= 0.01; break;
        }

        for (int i = 0; i < HARMONICS_COUNT; i++)
            std::cout << (i+1) << " " << harmonics_amplitude[i] << std::endl;

        std::cout << std::endl;
        std::cout << "DETUNE: " << detune << std::endl;
        std::cout << "PHASE:  " << phase << std::endl;
        std::cout << std::endl;
    }

    ma_device_uninit(&device);
    ma_context_uninit(&context);

    return 0;
}