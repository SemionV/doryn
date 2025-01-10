#pragma once

#include "registry.h"

namespace dory::core
{
    class ISetup
    {
    public:
        virtual ~ISetup() = default;

        virtual void setupRegistry(const generic::extension::LibraryHandle& libraryHandle, Registry& registry,
                                        const resources::configuration::Configuration& configuration) = 0;
    };
}