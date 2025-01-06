#pragma once

#include <chrono>
#include <vector>
#include <dory/core/resources/scene/scene.h>

namespace dory::core::resources::profiling
{
    struct Frame
    {
        std::chrono::nanoseconds duration;
        std::vector<scene::SceneViewStateSet> viewStates;
    };

    struct FrameSet
    {
        std::vector<Frame> frames;
    };
}
