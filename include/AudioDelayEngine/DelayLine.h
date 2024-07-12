#pragma once

#include <concepts>
#include <atomic>
#include "CircularBuffer.h"

template<std::floating_point FloatType = float>
class DelayLine
{
public:
    DelayLine(std::chrono::milliseconds delayTime, CircularBuffer<FloatType>& bufferToUse)  : time(delayTime), buffer(bufferToUse)
    {

    }

    ~DelayLine()
    {

    }

    std::chrono::milliseconds getDelayTime() const
    {
        return time;
    }

    //Threadsafe and realtime safe
    void setAmplitude(float newAmplitudeDb)
    {
        const float newGain = std::pow(10.0f, newAmplitudeDb / 20.0f);
        gain.store(newGain);
    }

private:
    //For main Delay class
    void process(std::span<FloatType> data)
    {
        wrap_around view = buffer.getReadBuffer(time);

        std::transform(data.begin(), data.end(), view.begin(), data.begin(), [this](FloatType first, FloatType second)
        {
            return first + second * gain;
        });
    }

    std::chrono::milliseconds time;
    CircularBuffer<FloatType>& buffer;

    std::atomic<float> gain = 1.0f;

    template<std::floating_point>
    friend class Delay;
};