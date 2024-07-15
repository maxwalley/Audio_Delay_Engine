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
    void setGain(double newGainDb)
    {
        const double newGain = std::pow(10.0, newGainDb / 20.0);
        gain.store(newGain);

        gainDb = newGainDb;
    }

    double getGain() const
    {
        return gainDb;
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

    std::atomic<double> gain = 1.0f;
    double gainDb = 0.0f;

    template<std::floating_point>
    friend class Delay;
};