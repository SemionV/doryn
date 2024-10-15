#pragma once

#include <dory/core/registry.h>
#include <memory>

namespace dory::core
{
    class Registry;
}

namespace dory::core::resources
{
    struct ExtensionContext
    {
        std::shared_ptr<core::Registry> registry;
    };

    struct ExecuteContext
    {
    };

}
