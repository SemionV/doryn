#pragma once

namespace dory::domain
{
    struct DataContext
    {
        bool isStop;

        DataContext():
            isStop(false)
        {}
    };
}