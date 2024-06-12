#include <concepts>
#include "CircularBuffer.h"

template<std::floating_point FloatType = float>
class Delay
{
public:
    Delay(std::chrono::seconds maxDelayTime)  : maxDelay(maxDelayTime)
    ~Delay();

    void prepare(int sampleRate, int bufferSize)
    {
        buffer = std::make_unique<CircularBuffer<FloatType>>(sampleRate, bufferSize, maxDelay);
    }

    void process(std::span<FloatType> data)
    {

    }

private:
    std::chrono::seconds maxDelay;
    std::unique_ptr<CircularBuffer<FloatType>> buffer = nullptr;
};