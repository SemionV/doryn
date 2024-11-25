#pragma once

namespace dory::core
{
    class Registry;

    class DependencyResolver
    {
    protected:
        Registry& _registry;

        explicit DependencyResolver(Registry& registry):
                _registry(registry)
        {}
    };
}
