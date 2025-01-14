#pragma once

#include <chrono>
#include <deque>
#include <vector>
#include <dory/core/resources/scene/scene.h>
#include <thread>
#include <chrono>
#include <readerwriterqueue.h>

namespace dory::core::resources::profiling
{
    struct TimeSlice
    {
        std::thread::id threadId;
        std::string name {};
        std::chrono::steady_clock::time_point begin;
        std::chrono::steady_clock::time_point end;
    };

    struct Frame
    {
        std::chrono::nanoseconds duration;
        std::vector<scene::SceneViewStateSet> viewStates;
        moodycamel::ReaderWriterQueue<TimeSlice> _queue {};
        std::size_t updatesCount {};
        float alpha = -1;
        std::size_t frameBufferBinding {};
        bool readFrameBufferIndex {};
        bool drawFrameBufferIndex {};
        std::string frontBufferImage {};
        std::string backBufferImage {};
    };

    struct Profiling
    {
        std::deque<Frame> frames {};
        IdType frontBufferStreamId = nullId;
        IdType backBufferStreamId = nullId;
    };
}
