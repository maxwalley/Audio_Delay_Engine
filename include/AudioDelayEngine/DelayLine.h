#include <cstdint>
#include <concepts>
#include <vector>
#include <atomic>

template<std::floating_point FloatType = float>
class DelayLine
{
public:
    DelayLine(float samplingRate, uint16_t delayTimeMs = 100)  : sampleRate(samplingRate), delayTime(delayTimeMs)
    {
        refreshBuffer();
    }

    ~DelayLine();

    //Not threadsafe
    void setSampleRate(float newSampleRate)
    {
        sampleRate = newSampleRate;
        refreshBuffer();
    }

    //Not threadsafe
    void setDelayTime(uint16_t newDelayTimeMs)
    {
        delayTime = newDelayTimeMs;
        refreshBuffer();
    }

    //Threadsafe and realtime safe
    void setAmplitude(float newAmplitudeDb)
    {
        const float newGain = std::pow(10.0f, newAmplitudeDb / 20.0f);
        gain.store(newGain);
    }

    //void process(std::span<FloatType> samples);
    FloatType process(FloatType sample)
    {
        buffer[writePos++] = sample;

        if(writePos >= buffer.size())
        {
            writePos = 0;
        }

        const FloatType ret = buffer[readPos++];

        if(readPos >= buffer.size())
        {
            readPos = 0;
        }

        return ret * gain.load();
    }

private:
    void refreshBuffer()
    {
        const size_t bufferSizeSamples = (sampleRate / 1000.0f) * delayTime;

        buffer.resize(bufferSizeSamples);
        std::fill(buffer.begin(), buffer.end(), 0.0f);

        readPos = 1;
        writePos = 0;
    }

    std::vector<FloatType> buffer;
    size_t writePos;
    size_t readPos;

    float sampleRate;
    uint16_t delayTime;

    std::atomic<float> gain = 1.0f;
};