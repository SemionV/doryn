#pragma once

#include "../entity.h"
#include "../name.h"
#include "..//windowSystem.h"
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
