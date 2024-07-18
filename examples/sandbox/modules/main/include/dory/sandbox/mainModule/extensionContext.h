#pragma once

#include "registry.h"

namespace dory::sandbox
{
    struct ExtensionContext
    {
        Registry& registry;

        ExtensionContext(Registry& registry):
            registry(registry)
        {}
    };
}