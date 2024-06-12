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
        buffer.resize(maxDelay.count() * sampleRate + buffersize);
    }

    wrap_around getWriteBuffer()
    {
        auto buffer = wrap_around(buffer, writePos, buffersize);

        writePos += buffersize;

        return buffer;
    }

    wrap_around getReadBuffer(std::chrono::milliseconds delayLength)
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
    std::vector<FloatType> buffer;
    size_t writePos = 0;
};