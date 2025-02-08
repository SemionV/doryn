#pragma once

namespace dory::core::resources
{
    struct Duration
    {
        std::optional<float> nanoseconds {};
        std::optional<float> microseconds {};
        std::optional<float> milliseconds {};
        std::optional<float> seconds {};
        std::optional<float> minutes {};
    };

    inline void getDuration(const Duration& duration, std::chrono::nanoseconds& nanoseconds)
    {
        if(duration.nanoseconds)
        {
            const std::chrono::duration<float, std::nano> value { *duration.nanoseconds };
            nanoseconds = std::chrono::duration_cast<std::chrono::nanoseconds>(value);
        }
        else if(duration.microseconds)
        {
            const std::chrono::duration<float, std::micro> value { *duration.microseconds };
            nanoseconds = std::chrono::duration_cast<std::chrono::nanoseconds>(value);
        }
        else if(duration.milliseconds)
        {
            const std::chrono::duration<float, std::milli> value { *duration.milliseconds };
            nanoseconds = std::chrono::duration_cast<std::chrono::nanoseconds>(value);
        }
        else if(duration.seconds)
        {
            const std::chrono::duration<float> value{ *duration.seconds };
            nanoseconds = std::chrono::duration_cast<std::chrono::nanoseconds>(value);
        }
        else if(duration.minutes)
        {
            const std::chrono::duration<float, std::ratio<60>> value{ *duration.minutes };
            nanoseconds = std::chrono::duration_cast<std::chrono::nanoseconds>(value);
        }
    }
}
