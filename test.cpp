#include <gtest/gtest.h>
#include <AudioDelayEngine/Delay.h>

using namespace std::chrono_literals;

TEST(DelayEngineTests, VerifyMaxDelayTime)
{
    //Test that max delay time is as set
    constexpr std::chrono::seconds firstMaxDelayTime = 30s;
    Delay firstDelay(firstMaxDelayTime);
    
    EXPECT_EQ(firstDelay.getMaxDelayTime(), firstMaxDelayTime);

    constexpr std::chrono::seconds secondMaxDelayTime = 500s;
    Delay secondDelay(secondMaxDelayTime);

    EXPECT_EQ(secondDelay.getMaxDelayTime(), secondMaxDelayTime);
}

TEST(DelayEngineTests, AddLines)
{
    //Test adding lines then test size
    Delay delay(30s);
    delay.addLine(500ms);

    ASSERT_EQ(delay.numLines(), 1);

    delay.addLine(300ms);
    delay.addLine(2s);

    ASSERT_EQ(delay.numLines(), 3);
}

TEST(DelayEngineTests, AddLineTooLongDelayTime)
{
    //Test adding a line with a too long delay time
    Delay delay(5s);
    delay.addLine(6s);

    ASSERT_EQ(delay.numLines(), 0);

    delay.addLine(300ms);
    delay.addLine(70s);

    ASSERT_EQ(delay.numLines(), 1);
}

TEST(DelayEngineTests, RemoveLine)
{
    //Test removing line
    Delay delay(3s);
    delay.addLine(1500ms);
    delay.addLine(300ms);
    ASSERT_EQ(delay.numLines(), 2);

    delay.removeLine(0);
    ASSERT_EQ(delay.numLines(), 1);

    //Check that left over line is the correct one
    const DelayLine<float>* leftOverLine = delay.getLine(0);
    ASSERT_NE(nullptr, leftOverLine);

    ASSERT_EQ(leftOverLine->getDelayTime(), 300ms);
}

TEST(DelayEngineTests, RemoveNonExistantLine)
{
    //Test removing line that does not exist
    Delay delay(3s);
    delay.addLine(1500ms);
    delay.addLine(300ms);
    ASSERT_EQ(delay.numLines(), 2);

    delay.removeLine(3);
    ASSERT_EQ(delay.numLines(), 2);
}

TEST(DelayEngineTests, RetrieveLine)
{
    //Test adding a line then retrieving it
    Delay delay(5s);
    delay.addLine(1s);
    ASSERT_EQ(delay.numLines(), 1);

    DelayLine<float>* line = delay.getLine(0);
    ASSERT_NE(line, nullptr);

    const DelayLine<float>* constLine = delay.getLine(0);
    ASSERT_NE(constLine, nullptr);
}

TEST(DelayEngineTests, RetrieveNonExistantLine)
{
    //Test retrieving a line that does not exist
    Delay delay(3s);
    delay.addLine(100ms);
    ASSERT_EQ(delay.numLines(), 1);

    DelayLine<float>* line = delay.getLine(3);
    ASSERT_EQ(line, nullptr);

    const DelayLine<float>* constLine = delay.getLine(3);
    ASSERT_EQ(constLine, nullptr);
}

TEST(DelayEngineTests, VerifyLineDelayTime)
{
    //Test adding a line and that the delay time is consistent
    constexpr std::chrono::milliseconds delayTime = 10ms;
    
    Delay delay(1s);
    delay.addLine(delayTime);
    ASSERT_EQ(delay.numLines(), 1);

    const DelayLine<float>* line = delay.getLine(0);
    ASSERT_NE(line, nullptr);

    ASSERT_EQ(line->getDelayTime(), delayTime);
}

TEST(DelayEngineTests, ImpulseTestSingleBuffer)
{
    constexpr int sampleRate = 44100;
    constexpr int bufferSize = 512;
    std::vector<float> buffer(bufferSize);
    constexpr float sampleVal = 1.0f;
    buffer[0] = sampleVal;

    constexpr std::chrono::milliseconds delayTime(10);
    constexpr int delayTimeSamples = (sampleRate / 1000.0) * delayTime.count();
    ASSERT_TRUE(delayTimeSamples < bufferSize);

    Delay delay(1s);
    delay.addLine(delayTime);
    delay.prepare(sampleRate, bufferSize);
    delay.process({buffer.data(), buffer.size()});

    for(int sampleIndex = 0; sampleIndex < bufferSize; ++sampleIndex)
    {
        const float expectedSampleVal = sampleIndex == delayTimeSamples ? sampleVal : 0.0f;
        EXPECT_EQ(buffer[sampleIndex], expectedSampleVal);
    }
}

TEST(DelayEngineTests, ImpulseTestMultiBuffer)
{
    constexpr int sampleRate = 44100;
    constexpr int bufferSize = 512;
    std::vector<float> buffer(bufferSize);
    constexpr float sampleVal = 1.0f;
    buffer[0] = sampleVal;

    constexpr std::chrono::milliseconds delayTime(1000);
    constexpr int delayTimeSamples = (sampleRate / 1000.0) * delayTime.count();
    ASSERT_TRUE(delayTimeSamples > bufferSize);

    Delay delay(2s);
    delay.addLine(delayTime);
    delay.prepare(sampleRate, bufferSize);
    delay.process({buffer.data(), buffer.size()});

    for(int bufferIndex = 0; bufferIndex < sampleRate / bufferSize + 2; ++bufferIndex)
    {
        for(size_t sampleIndex = 0; sampleIndex < bufferSize; ++sampleIndex)
        {
            size_t overallSampleIndex = bufferIndex * bufferSize + sampleIndex;

            const float expectedSampleVal = overallSampleIndex == delayTimeSamples ? sampleVal : 0.0f;
            EXPECT_EQ(buffer[sampleIndex], expectedSampleVal);
        }

        std::fill(buffer.begin(), buffer.end(), 0.0f);
        delay.process({buffer.data(), buffer.size()});
    }
}

TEST(DelayEngineTests, ImpulseTestMultiLine)
{
    constexpr int sampleRate = 44100;
    constexpr int bufferSize = 512;
    std::vector<float> buffer(bufferSize);
    constexpr float sampleVal = 1.0f;
    buffer[0] = sampleVal;

    constexpr std::chrono::milliseconds lineOneDelayTime(50);
    constexpr int lineOneDelayTimeSamples = (sampleRate / 1000.0) * lineOneDelayTime.count();

    constexpr std::chrono::milliseconds lineTwoDelayTime(120);
    constexpr int lineTwoDelayTimeSamples = (sampleRate / 1000.0) * lineTwoDelayTime.count();

    Delay delay(1s);
    delay.addLine(lineOneDelayTime);
    delay.addLine(lineTwoDelayTime);
    delay.prepare(sampleRate, bufferSize);
    delay.process({buffer.data(), buffer.size()});


    for(int bufferIndex = 0; bufferIndex < sampleRate / bufferSize + 2; ++bufferIndex)
    {
        for(size_t sampleIndex = 0; sampleIndex < bufferSize; ++sampleIndex)
        {
            size_t overallSampleIndex = bufferIndex * bufferSize + sampleIndex;

            const float expectedSampleVal = (overallSampleIndex == lineOneDelayTimeSamples || overallSampleIndex == lineTwoDelayTimeSamples) ? sampleVal : 0.0f;
            EXPECT_EQ(buffer[sampleIndex], expectedSampleVal);
        }

        std::fill(buffer.begin(), buffer.end(), 0.0f);
        delay.process({buffer.data(), buffer.size()});
    }
}

TEST(DelayEngineTests, ImpulseTestZeroDelay)
{
    constexpr int sampleRate = 44100;
    constexpr int bufferSize = 512;
    std::vector<float> buffer(bufferSize);
    constexpr float sampleVal = 1.0f;
    buffer[0] = sampleVal;

    Delay delay(1s);
    delay.addLine(0ms);
    delay.prepare(sampleRate, bufferSize);
    delay.process({buffer.data(), buffer.size()});

    for(size_t sampleIndex = 0; sampleIndex < buffer.size(); ++sampleIndex)
    {
        const float expectedSampleVal = sampleIndex == 0 ? sampleVal : 0.0f;
        EXPECT_EQ(buffer[sampleIndex], expectedSampleVal);
    }
}

//RECIRCULATION TESTS
//GAIN TESTS ON DELAY LINES