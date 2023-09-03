#pragma once

namespace dory
{
    struct Matrix4x4
    {
        float entries[16];

        Matrix4x4():
            entries{1.f, 0.f, 0.f, 0.f,
                    0.f, 1.f, 0.f, 0.f,
                    0.f, 0.f, 1.f, 0.f,
                    0.f, 0.f, 0.f, 1.f}
        {            
        }
    };
}