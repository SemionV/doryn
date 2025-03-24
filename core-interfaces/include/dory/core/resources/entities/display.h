#pragma once

#include "../entity.h"
#include "../name.h"
#include "layout.h"

namespace dory::core::resources::entities
{
    struct Display: public Entity<>
    {
        Name name {};
        layout::Size size {};
        int refreshRate {};
        bool isPrimary{};
    };
}
