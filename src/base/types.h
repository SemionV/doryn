#pragma once

#include "doryExport.h"

namespace dory
{
    enum UnitInverseDevider
    {
        Identity = 1,
        Milli = 1000,
        Micros = 1000000,
        Nano = 1000000000
    };

    struct DORY_API TimeSpan
    {
        long duration;
        UnitInverseDevider unitsPerSecond;

        TimeSpan();
        TimeSpan(UnitInverseDevider unitsPerSecond);
    };

    class DORY_API TimeConverter
    {
        public:
            static double ToMilliseconds(TimeSpan timeSpan);
    };
}