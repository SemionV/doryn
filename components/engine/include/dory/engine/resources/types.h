#pragma once

namespace dory::engine::resources
{
    enum UnitScale
    {
        Identity = 1,
        Milli = 1000,
        Micro = 1000000,
        Nano = 1000000000
    };

    struct TimeSpan
    {
        long duration;
        UnitScale unitsPerSecond;

        TimeSpan():
            duration(0),
            unitsPerSecond(UnitScale::Milli)
        {
        };

        explicit TimeSpan(UnitScale unitsPerSecond):
            duration(0),
            unitsPerSecond(unitsPerSecond)
        {
        };

        [[nodiscard]] double ToMilliseconds() const
        {
            return (duration / (double)unitsPerSecond) * (double)UnitScale::Milli;
        }
    };

    class TimeConverter
    {
        public:
            static double ToMilliseconds(TimeSpan timeSpan)
            {
                return (timeSpan.duration / (double)timeSpan.unitsPerSecond) * (double)UnitScale::Milli;
            }
    };

    struct Color
    {
        float r {1.0f};
        float g {1.0f};
        float b {1.0f};
        float a {1.0f};

        Color() = default;

        Color(float r, float g, float b, float a = 1.0f):
            r(r), g(g), b(b), a(a)
        {}
    };
}