#pragma once

#include "libraryHandle.h"
#include "iLibrary.h"

namespace dory::generic::extension
{
    template<typename TDataContext>
    class IModule
    {
    public:
        virtual ~IModule() = default;

        virtual void attach(LibraryHandle library, TDataContext& dataContext) = 0;
    };
}