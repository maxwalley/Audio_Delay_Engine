#pragma once

#include <concepts>
#include "WrapAroundView.h"

template<std::floating_point FloatType = float>
class CircularBuffer
{
public:
    CircularBuffer(std::chrono::seconds maxDelayTime)  : maxDelay(maxDelayTime)
    {
        
    }

    ~CircularBuffer() {}

    void prepare(int sampleRate, int bufferSize)
    {
        buffersize = bufferSize;
        samplerate = sampleRate;
        buffer.resize(maxDelay.count() * samplerate + buffersize);
    }

    wrap_around<std::vector<FloatType>> getWriteBuffer()
    {
        auto wrapper = wrap_around(buffer, writePos, buffersize);

        writePos += buffersize;

        return wrapper;
    }

    wrap_around<std::vector<FloatType>> getReadBuffer(std::chrono::milliseconds delayLength)
    {
        size_t delaySamples = (samplerate / 1000.0) * delayLength.count();
        size_t delayPos;

        if(writePos - buffersize - delaySamples < 0)
        {
            delayPos = buffer.size() - delaySamples - writePos;
        }
        else
        {
            delayPos = writePos - buffersize - delaySamples;
        }

        return wrap_around(buffer, delayPos, buffersize);
    }

private:
    std::chrono::seconds maxDelay;
    int buffersize;
    int samplerate;
    std::vector<FloatType> buffer;
    size_t writePos = 0;
};