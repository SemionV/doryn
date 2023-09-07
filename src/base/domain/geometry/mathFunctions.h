#pragma once

namespace dory
{
    class MathFunctions
    {
        public:
            static constexpr const double PiNumber = 3.14159265358979323846;

            static const float getRadians(float degrees)
            {
                return degrees * (PiNumber / 180);
            }
    };
}