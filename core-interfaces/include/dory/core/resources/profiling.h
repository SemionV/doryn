#pragma once

#include <chrono>
#include <stack>
#include <vector>
#include <dory/core/resources/scene/scene.h>
#include <thread>
#include <stack>

namespace dory::core::resources::profiling
{
    struct TimeSlice
    {
        std::string name {};
        std::chrono::steady_clock::time_point begin;
        std::chrono::steady_clock::time_point end;
        std::deque<TimeSlice> subTimeSlices;
    };

    struct Timing
    {
        std::stack<TimeSlice*> stack;
        TimeSlice root;
    };

    struct Frame
    {
        std::size_t id {};
        std::vector<scene::SceneViewStateSet> viewStates;
        std::unordered_map<std::thread::id, Timing> timings;
        std::size_t updatesCount {};
        float alpha = -1;
        std::size_t frameBufferBinding {};
        bool readFrameBufferIndex {};
        bool drawFrameBufferIndex {};
        std::string frontBufferImage {};
        std::string backBufferImage {};
    };

    struct Capture
    {
        std::size_t id;
        std::size_t framesCountLimit = 100;
        std::deque<Frame> frames {};
        std::atomic<bool> done { };

        Capture(const std::size_t captureId, const std::size_t countLimit)
        : id(captureId), framesCountLimit(countLimit)
        {}

        Capture(Capture&& other) noexcept
        : id(other.id),
          framesCountLimit(other.framesCountLimit),
          frames(std::move(other.frames)),
          done(other.done.load())
        {}

        Capture& operator=(Capture&& other) noexcept
        {
            if (this != &other) {
                id = other.id;
                framesCountLimit = other.framesCountLimit;
                frames = std::move(other.frames);
                done.store(other.done.load());
            }
            return *this;
        }

        Capture(const Capture&) = delete;
        Capture& operator=(const Capture&) = delete;
    };

    struct Profiling
    {
        bool captureFrameBuffers {};
        std::vector<Capture> captures;
        std::size_t captureIdCounter {};
        static constexpr std::size_t maxFramesCapture = 100;
        static constexpr std::string_view frameRootTimeSlice = "frame";
        IdType frontBufferStreamId = nullId;
        IdType backBufferStreamId = nullId;
    };

    inline Capture* startNewCapture(Profiling& profiling, const std::size_t captureId, const std::size_t framesCountLimit = 100)
    {
        return &profiling.captures.emplace_back( captureId, framesCountLimit );
    }

    inline void removeCurrentCapture(Profiling& profiling)
    {
        if(!profiling.captures.empty())
        {
            profiling.captures.pop_back();
        }
    }

    inline Capture* getCurrentCapture(Profiling& profiling)
    {
        if(!profiling.captures.empty())
        {
            return &profiling.captures.back();
        }

        return nullptr;
    }

    inline Frame* getCurrentFrame(Capture& capture)
    {
        if(!capture.frames.empty())
        {
            return &capture.frames.back();
        }

        return nullptr;
    }

    inline Frame* getCurrentFrame(Profiling& profiling)
    {
        if(auto* capture = getCurrentCapture(profiling))
        {
            return getCurrentFrame(*capture);
        }

        return nullptr;
    }

    inline const TimeSlice* getRootTimeSlice(const Frame& frame)
    {
        const auto it = frame.timings.find(std::this_thread::get_id());
        if(it != frame.timings.end())
        {
            return &it->second.root;
        }

        return nullptr;
    }

    inline TimeSlice* getCurrentTimeSlice(Frame& frame)
    {
        const auto it = frame.timings.find(std::this_thread::get_id());
        if(it != frame.timings.end() && !it->second.stack.empty())
        {
            return it->second.stack.top();
        }

        return nullptr;
    }

    inline void pushTimeSlice(Frame& frame, const std::string& name, const std::chrono::steady_clock::time_point begin)
    {
        const auto it = frame.timings.find(std::this_thread::get_id());
        if(it != frame.timings.end() && !it->second.stack.empty())
        {
            auto& stack = it->second.stack;
            auto* parentTimeSlice = stack.top();

            parentTimeSlice->subTimeSlices.push_back({name, begin});
            stack.push(&parentTimeSlice->subTimeSlices.back());
        }

        //push root time slice on stack
        if(auto [it, success] = frame.timings.emplace(std::this_thread::get_id(), Timing{}); success)
        {
            auto& [stack, root] = it->second;
            stack.push(&root);
            root.name = name;
            root.begin = begin;
        }
    }

    inline void popTimeSlice(Frame& frame, const std::chrono::steady_clock::time_point end)
    {
        const auto it = frame.timings.find(std::this_thread::get_id());
        if(it != frame.timings.end() && !it->second.stack.empty())
        {
            auto& stack = it->second.stack;
            auto* timeSlice = stack.top();
            timeSlice->end = end;
            stack.pop();
        }
    }

    inline void pushTimeSlice(Profiling& profiling, const std::string& name, const std::chrono::steady_clock::time_point begin)
    {
        if(auto* capture = getCurrentCapture(profiling))
        {
            if(auto* frame = getCurrentFrame(*capture))
            {
                pushTimeSlice(*frame, name, begin);
            }
        }
    }

    inline void popTimeSlice(Profiling& profiling, const std::chrono::steady_clock::time_point end)
    {
        auto* capture = getCurrentCapture(profiling);
        if(capture)
        {
            if(auto* frame = getCurrentFrame(*capture))
            {
                popTimeSlice(*frame, end);
            }
        }
    }


    inline Frame* addNewFrame(Capture& capture, const std::size_t frameId)
    {
        if(!capture.done)
        {
            auto& frame = capture.frames.emplace_back();
            frame.id = frameId;

            while(capture.frames.size() > capture.framesCountLimit)
            {
                capture.frames.pop_front();
            }

            return &frame;
        }

        return nullptr;
    }
}
