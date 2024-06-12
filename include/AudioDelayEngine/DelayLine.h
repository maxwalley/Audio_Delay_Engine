#include <concepts>
#include <atomic>
#include "CircularBuffer.h"

template<std::floating_point FloatType = float>
class DelayLine
{
public:
    DelayLine(CircularBuffer& buf, std::chrono::milliseconds delayTime)  : buffer(buf), time(delayTime)
    {

    }

    ~DelayLine()
    {

    }

    void process(std::span<FloatType> data)
    {
        
    }

    //Threadsafe and realtime safe
    void setAmplitude(float newAmplitudeDb)
    {
        const float newGain = std::pow(10.0f, newAmplitudeDb / 20.0f);
        gain.store(newGain);
    }

private:
    CircularBuffer<FloatType>& buffer;
    std::chrono::milliseconds time;

    std::atomic<float> gain = 1.0f;
};