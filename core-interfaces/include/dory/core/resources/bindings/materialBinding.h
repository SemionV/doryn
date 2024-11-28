#pragma once

#include <dory/core/resources/entity.h>

namespace dory::core::resources::bindings
{
    struct MaterialBinding: Entity<>
    {
        std::string linkingError{};
        math::Vector4f color {};
    };
}