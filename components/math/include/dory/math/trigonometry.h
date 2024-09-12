#pragma once

namespace dory::math
{
    static constexpr const double PiNumber = 3.14159265358979323846;

    template<typename T>
    static auto getRadians(T degrees)
    {
        return degrees * (PiNumber / 180);
    }
}
