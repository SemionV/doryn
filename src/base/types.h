#pragma once

#include "doryExport.h"

namespace dory
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

        TimeSpan();
        TimeSpan(UnitScale unitsPerSecond);
    };

    class DORY_API TimeConverter
    {
        public:
            static double ToMilliseconds(TimeSpan timeSpan);
    };
}