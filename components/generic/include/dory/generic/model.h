#pragma once

namespace dory::generic::model
{
    using TimeSpan = std::chrono::duration<float>;

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
