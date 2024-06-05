#include <concepts>
#include "WrapAroundView.h"

template<std::floating_point FloatType = float>
class CircularBuffer
{
public:
    CircularBuffer(int sampleRate, int bufferSize, std::chrono::seconds maxDelayTime)  : samplerate(sampleRate), buffersize(bufferSize)
    {
        buffer.resize(maxDelayTime.count() * samplerate + bufferSize);
    }

    ~CircularBuffer() {}

    wrap_around getWriteBuffer()
    {
        return wrap_around(buffer, writePos, buffersize);
    }

    wrap_around getReadBuffer(std::chrono::milliseconds delayLength)
    {
        size_t delaySamples = (samplerate / 1000.0) * delayLength.count();
        size_t delayPos;

        if(writePos - delaySamples < 0)
        {
            delayPos = buffer.size() - delaySamples - writePos;
        }
        else
        {
            delayPos = writePos - delaySamples;
        }

        return wrap_around(buffer, delayPos, buffersize);
    }

private:
    int samplerate;
    int buffersize;
    std::vector<FloatType> buffer;
    size_t writePos = 0;
};