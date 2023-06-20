#pragma once

namespace dory
{
    enum TimeFractionOfSecond
    {
        Second = 1,
        Millisecond = 1000,
        Microsecond = 1000000,
        Nanosecond = 1000000000
    };

    struct TimeSpan
    {
        long duration;
        TimeFractionOfSecond unitsPerSecond;

        TimeSpan(TimeFractionOfSecond unitsPerSecond):
            duration(0),
            unitsPerSecond(unitsPerSecond)
        {
        }
    };
}