#pragma once

#include <chrono>
#include <deque>
#include <vector>
#include <dory/core/resources/scene/scene.h>

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
        std::vector<TimeSlice> timeSlices;
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
}
