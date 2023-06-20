#include "types.h"

namespace dory
{
    TimeSpan::TimeSpan():
            duration(0),
            unitsPerSecond(TimeUnit::Millisecond)
    {
    }

    TimeSpan::TimeSpan(TimeUnit unitsPerSecond):
            duration(0),
            unitsPerSecond(unitsPerSecond)
    {
    }

    inline double TimeConverter::ToMilliseconds(TimeSpan timeSpan)
    {
        return (timeSpan.duration / (double)timeSpan.unitsPerSecond) * TimeUnit::Millisecond;
    }
}