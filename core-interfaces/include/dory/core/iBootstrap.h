#pragma once

#include <dory/generic/baseTypes.h>
#include <dory/generic/extension/libraryHandle.h>

namespace dory::core
{
    class Registry;

    class IBootstrap: public generic::Interface
    {
    public:
        virtual bool initialize(const dory::generic::extension::LibraryHandle& libraryHandle, resources::DataContext& context) = 0;
        virtual bool run(resources::DataContext& context) = 0;
        virtual void cleanup(resources::DataContext& context) = 0;
    };
}
