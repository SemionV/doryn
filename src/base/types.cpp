#include "types.h"

namespace dory
{
    TimeSpan::TimeSpan():
            duration(0),
            unitsPerSecond(UnitInverseDevider::Milli)
    {
    }

    TimeSpan::TimeSpan(UnitInverseDevider unitsPerSecond):
            duration(0),
            unitsPerSecond(unitsPerSecond)
    {
    }

    inline double TimeConverter::ToMilliseconds(TimeSpan timeSpan)
    {
        return (timeSpan.duration / (double)timeSpan.unitsPerSecond) * UnitInverseDevider::Milli;
    }
}