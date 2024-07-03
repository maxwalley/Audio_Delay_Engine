#pragma once

#include <concepts>
#include <algorithm>
#include "CircularBuffer.h"
#include "DelayLine.h"

template<std::floating_point FloatType = float>
class Delay
{
public:
    Delay(std::chrono::seconds maxDelayTime)  : maxDelay(maxDelayTime) {}
    ~Delay() {}

    void addLine(std::chrono::milliseconds lineDelayTime)
    {

    }

    DelayLine<FloatType>* getLine(size_t lineIndex)
    {

    }

    const DelayLine<FloatType>* getLine(size_t lineIndex) const
    {

    }

    size_t numLines() const
    {

    }

    void prepare(int sampleRate, int bufferSize)
    {
        buffer = std::make_unique<CircularBuffer<FloatType>>(sampleRate, bufferSize, maxDelay);

        std::for_each(lines.begin(), lines.end(), [this](DelayLine<FloatType>& line)
        {
            line.prepare(buffer.get());
        });
    }

    void process(std::span<FloatType> data)
    {

    }

private:
    std::chrono::seconds maxDelay;
    std::unique_ptr<CircularBuffer<FloatType>> buffer = nullptr;

    std::vector<DelayLine<FloatType>> lines;
};