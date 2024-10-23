#pragma once

#include "../../../../../components/generic/include/dory/generic/extension/libraryHandle.h"

namespace dory::core
{
    class Registry;

    namespace resources
    {
        class DataContext;
    }

    namespace extensionPlatform
    {
        class IModule
        {
        public:
            virtual ~IModule() = default;

            virtual void attach(LibraryHandle library, resources::DataContext& dataContext) = 0;
        };
    }
}