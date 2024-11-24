#pragma once

#include "id.h"

namespace dory::core::resources
{
    template<typename T = IdType>
    struct Entity
    {
        using IdType = IdType;

        T id {};

        Entity() = default;

        Entity(T id):
                id(id)
        {}
    };
}