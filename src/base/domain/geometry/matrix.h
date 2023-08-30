#pragma once

namespace dory
{
    struct Matrix4x4
    {
        float entries[16];

        Matrix4x4():
            entries{1, 0, 0, 0,
                    0, 1, 0, 0,
                    0, 0, 1, 0,
                    0, 0, 0, 1}
        {            
        }
    };
}