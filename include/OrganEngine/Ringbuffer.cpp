#include "RingBuffer.h"

// From BerVibrato
// https://github.com/Bershov/Vibrato-effect/tree/master

RingBuffer::RingBuffer()
: writeIndex(0), size(0)
{}

void RingBuffer::resize(int sz)
{
	size = sz;
	buffer.resize(size + interpolatorMargin);
}

void RingBuffer::write(float sample)
{
	buffer[writeIndex] = sample;
	writeIndex++;
	if(writeIndex == size){
		writeIndex = 0;
	}
}

void RingBuffer::write_margined(int32_t sample)
{
    buffer[writeIndex] = sample;

    if( writeIndex < interpolatorMargin )
    {
        buffer[size + writeIndex] = sample;
    }

    writeIndex++;
    if(writeIndex == size){
        writeIndex = 0;
    }
}

int32_t RingBuffer::readWithDelay(int delay){
	int readIndex = writeIndex - delay;
	if (readIndex < 0){
		readIndex += size;
	}
	return buffer[readIndex];
}
