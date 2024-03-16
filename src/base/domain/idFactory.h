#pragma once

#include "base/typeComponents.h"

namespace dory::domain
{
    template<typename TId>
    class IdFactory: Uncopyable
    {
    private:
        TId counter;

    public:
        static const constexpr TId nullId{};

    public:
        TId generate()
        {
            return ++counter;
        }
    };
}