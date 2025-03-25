#pragma once

#include "../entity.h"
#include "../name.h"
#include <dory/core/resources/systemTypes.h>
#include "layout.h"

namespace dory::core::resources::entities
{
    struct Display: public Entity<>
    {
        DisplaySystem displaySystem;
        Name name {};
        layout::Size size {};
        int refreshRate {};
        bool isPrimary{};
    };
}
