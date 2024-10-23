#pragma once

#include "libraryHandle.h"
#include "iLibrary.h"
#include "dory/core/resources/dataContext.h"

namespace dory::core::extensionPlatform
{
    class IModule
    {
    public:
        virtual ~IModule() = default;

        virtual void attach(LibraryHandle library, resources::DataContext& dataContext) = 0;
    };
}