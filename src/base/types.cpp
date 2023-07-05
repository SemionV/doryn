#include "types.h"

namespace dory
{
    TimeSpan::TimeSpan():
            duration(0),
            unitsPerSecond(UnitScale::Milli)
    {
    }

    TimeSpan::TimeSpan(UnitScale unitsPerSecond):
            duration(0),
            unitsPerSecond(unitsPerSecond)
    {
    }

    double TimeSpan::ToMilliseconds() const
    {
        return (duration / (double)unitsPerSecond) * UnitScale::Milli;
    }

    inline double TimeConverter::ToMilliseconds(TimeSpan timeSpan)
    {
        return (timeSpan.duration / (double)timeSpan.unitsPerSecond) * UnitScale::Milli;
    }
}