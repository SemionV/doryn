#pragma once

#include "libraryHandle.h"

namespace dory::core
{
    class Registry;

    namespace resources
    {
        class DataContext;
    }
}

namespace dory::core::extensionPlatform
{
    class IModule
    {
    public:
        virtual ~IModule() = default;

        virtual void attach(LibraryHandle library, resources::DataContext& dataContext, Registry& registry) = 0;
    };
}
