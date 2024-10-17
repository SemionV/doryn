#pragma once

#include <memory>

namespace dory::core
{
    class Registry;
}

namespace core
{
    class Registry;
}

namespace dory::core::resources
{
    class DataContext;

    struct ExtensionContext
    {
        Registry& registry;
        DataContext& dataContext;
    };

    struct ExecuteContext
    {
    };

}
