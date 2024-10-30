#pragma once

#include <dory/generic/baseTypes.h>
#include <dory/generic/extension/libraryHandle.h>

namespace dory::core
{
    class Registry;

    class IBootstrap: public generic::Interface
    {
    public:
        virtual bool initialize(const dory::generic::extension::LibraryHandle& libraryHandle, resources::DataContext& context, Registry& registry) = 0;
        virtual bool run(resources::DataContext& context, Registry& registry) = 0;
    };
}
