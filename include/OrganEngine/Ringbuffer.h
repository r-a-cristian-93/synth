#ifndef __RINGBUFFER_H
#define __RINGBUFFER_H

// From BerVibrato
// https://github.com/Bershov/Vibrato-effect/tree/master

#ifdef _MSC_VER
#define INLINE __forceinline
#else
#define INLINE inline
#endif

#include <vector>
#include <cstdint>

// Hermite polynomial interpolation.
// More points - better response at high frequencies
__attribute__((always_inline)) inline
int32_t getSampleHermite4p3o(float x, int32_t *y)
{
    static float c0, c1, c2, c3;

    // 4-point, 3rd-order Hermite (x-form)
    c0 = (float)y[1];
    c1 = (1.0/2.0)*((float)y[2]-(float)y[0]);
    c2 = ((float)y[0] - (5.0/2.0)*(float)y[1]) + (2.0*(float)y[2] - (1.0/2.0)*(float)y[3]);
    c3 = (1.0/2.0)*((float)y[3]-(float)y[0]) + (3.0/2.0)*((float)y[1]-(float)y[2]);
    return (int32_t) (((c3*x+c2)*x+c1)*x+c0);
}

class RingBuffer
{
public:
	RingBuffer();

    void write(float sample);
    void write_margined(int32_t sample);
    int32_t readWithDelay(int delay);
	void resize(int size);

public:
	__attribute__((always_inline)) inline
    float getHermiteAt(float delay)
    {
        float fReadIndex = writeIndex - 1 - delay;
        while(fReadIndex < 0)
            fReadIndex += size;
        while(fReadIndex >= size)
            fReadIndex -= size;

        int iPart = (int)fReadIndex; // integer part of the delay
        float fPart = fReadIndex - iPart; // fractional part of the delay

        return getSampleHermite4p3o(fPart, &(buffer[iPart]));
    }

private:
	std::vector<int32_t>  buffer;
	int                 writeIndex;
	int                 size;

    static const int interpolatorMargin = 3;
};

#endif
