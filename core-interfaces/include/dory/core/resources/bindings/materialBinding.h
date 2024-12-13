#pragma once

#include <dory/core/resources/entity.h>
#include "../objects/materialProperties.h"

namespace dory::core::resources::bindings
{
    struct MaterialBinding: Entity<>
    {
        std::string linkingError{};
        assets::Material material {};
    };
}