#pragma once

#include "registry.h"

namespace dory::sandbox
{
    struct ExtensionContext
    {
        Registry& registry;
    };
}