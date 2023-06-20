#pragma once

#include "doryExport.h"

namespace dory
{
    enum TimeUnit
    {
        Second = 1,
        Millisecond = 1000,
        Microsecond = 1000000,
        Nanosecond = 1000000000
    };

    struct DORY_API TimeSpan
    {
        long duration;
        TimeUnit unitsPerSecond;

        TimeSpan();
        TimeSpan(TimeUnit unitsPerSecond);
    };

    class DORY_API TimeConverter
    {
        public:
            static double ToMilliseconds(TimeSpan timeSpan);
    };
}