#pragma once

#include <chrono>
#include <deque>
#include <vector>
#include <dory/core/resources/scene/scene.h>
#include <thread>
#include <readerwriterqueue.h>
#include <concurrentqueue.h>

namespace dory::core::resources::profiling
{
    struct TimeSlice
    {
        std::string name {};
        std::chrono::nanoseconds duration {};
    };

    struct Frame
    {
        std::chrono::nanoseconds duration;
        std::vector<scene::SceneViewStateSet> viewStates;
        moodycamel::ConcurrentHashMap<int, std::string> map;
        std::unordered_map<std::thread::id, TimeSlice> timeSlices; //TODO: use concurrent hash map data structure
        std::size_t updatesCount {};
        float alpha = -1;
        std::size_t frameBufferBinding {};
        bool readFrameBufferIndex {};
        bool drawFrameBufferIndex {};
    };

    struct FrameSet
    {
        std::deque<Frame> frames {};
    };

    struct Profiling
    {
        std::deque<FrameSet> frameSets;
    };
}
