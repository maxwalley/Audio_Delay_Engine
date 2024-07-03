#include <gtest/gtest.h>
#include <AudioDelayEngine/Delay.h>

using namespace std::chrono_literals;

TEST(DelayEngineTests, VerifyMaxDelayTime)
{
    //Test that max delay time is as set
    std::chrono::seconds firstMaxDelayTime = 30s;
    Delay firstDelay(firstMaxDelayTime);
    
    EXPECT_EQ(firstDelay.getMaxDelayTime(), firstMaxDelayTime);

    std::chrono::seconds secondMaxDelayTime = 500s;
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
    const std::chrono::milliseconds delayTime = 10ms;
    
    Delay delay(1s);
    delay.addLine(delayTime);
    ASSERT_EQ(delay.numLines(), 1);

    const DelayLine<float>* line = delay.getLine(0);
    ASSERT_NE(line, nullptr);

    ASSERT_EQ(line->getDelayTime(), delayTime);
}