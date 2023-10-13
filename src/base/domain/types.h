#pragma once

#include "base/doryExport.h"

namespace dory::domain
{
    enum UnitScale
    {
        Identity = 1,
        Milli = 1000,
        Micro = 1000000,
        Nano = 1000000000
    };

    struct DORY_API TimeSpan
    {
        long duration;
        UnitScale unitsPerSecond;

        TimeSpan():
            duration(0),
            unitsPerSecond(UnitScale::Milli)
        {
        };

        TimeSpan(UnitScale unitsPerSecond):
            duration(0),
            unitsPerSecond(unitsPerSecond)
        {
        };

        double ToMilliseconds() const
        {
            return (duration / (double)unitsPerSecond) * UnitScale::Milli;
        }
    };

    class DORY_API TimeConverter
    {
        public:
            static double ToMilliseconds(TimeSpan timeSpan)
            {
                return (timeSpan.duration / (double)timeSpan.unitsPerSecond) * UnitScale::Milli;
            }
    };

    struct Color
    {
        float r;
        float g;
        float b;
        float a;

        Color() = default;

        Color(float r, float g, float b, float a):
            r(r), g(g), b(b), a(a)
        {}
    };
}