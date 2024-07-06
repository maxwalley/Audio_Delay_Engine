#pragma once

#include <concepts>
#include <algorithm>
#include <cassert>
#include "CircularBuffer.h"
#include "DelayLine.h"

template<std::floating_point FloatType = float>
class Delay
{
public:
    Delay(std::chrono::seconds maxDelayTime)  : maxDelay(maxDelayTime), buffer(maxDelay)
    {
        lines = std::make_unique<LineList>();
        safeLines = lines.get();
    }

    ~Delay() {}

    std::chrono::seconds getMaxDelayTime() const
    {
        return maxDelay;
    }

    void addLine(std::chrono::milliseconds lineDelayTime)
    {
        if(lineDelayTime > maxDelay)
        {
            return;
        }

        std::unique_ptr<LineList> newLinesList = std::make_unique<LineList>();

        //Copy all lines across from main lines list
        std::copy(lines->begin(), lines->end(), std::back_inserter(*newLinesList));

        //Create new line and add to new lines list
        std::shared_ptr<DelayLine<FloatType>> newLine = std::make_shared<DelayLine<FloatType>>(lineDelayTime, buffer);

        newLinesList->push_back(newLine);

        //Wait for safe line list pointer to be out of real-time use and replace with the new list
        for(LineList* expected = lines.get(); !safeLines.compare_exchange_strong(expected, newLinesList.get()); expected = lines.get());

        //Replace main lines list with new list
        lines = std::move(newLinesList);
    }

    void removeLine(size_t lineIndex)
    {
        if(lineIndex >= lines->size())
        {
            return;
        }

        std::unique_ptr<LineList> newLinesList = std::make_unique<LineList>();

        //Copy all lines across from main lines list
        std::copy(lines->begin(), lines->end(), std::back_inserter(*newLinesList));

        //Remove line to delete from new list
        newLinesList->erase(newLinesList->begin() + lineIndex);

        //Wait for safe line list pointer to be out of real-time use and replace with the new list
        for(LineList* expected = lines.get(); !safeLines.compare_exchange_strong(expected, newLinesList.get()); expected = lines.get());

        //Replace main lines list with new list
        lines = std::move(newLinesList);
    }

    DelayLine<FloatType>* getLine(size_t lineIndex)
    {
        //We do not have to worry about thread safety here since this thread is the only one which can edit the lines list
        return lineIndex >= lines->size() ? nullptr : (*lines)[lineIndex].get();
    }

    const DelayLine<FloatType>* getLine(size_t lineIndex) const
    {
        //We do not have to worry about thread safety here since this thread is the only one which can edit the lines list
        return lineIndex >= lines->size() ? nullptr : (*lines)[lineIndex].get();
    }

    size_t numLines() const
    {
        //We do not have to worry about thread safety here since this thread is the only one which can edit the lines list
        return lines->size();
    }

    void prepare(int sampleRate, int bufferSize)
    {
        buffer.prepare(sampleRate, bufferSize);
    }

    void process(std::span<FloatType> data)
    {
        //CAS loop failed
        if(safeLines == nullptr)
        {
            assert(false);
            return;
        }

        //Mark that lines are now in use by real-time thread
        LineList* realtimeLines = safeLines.exchange(nullptr);

        //Mark that lines are now safe to edit
        safeLines = realtimeLines;
    }

private:
    std::chrono::seconds maxDelay;
    CircularBuffer<FloatType> buffer;

    using LineList = std::vector<std::shared_ptr<DelayLine<FloatType>>>;
    std::unique_ptr<LineList> lines;
    std::atomic<LineList*> safeLines;
};