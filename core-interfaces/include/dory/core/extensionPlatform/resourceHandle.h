#pragma once

#include "resourceRef.h"
#include "libraryHandle.h"

namespace dory::core::extensionPlatform
{
    template<typename TResource>
    class ResourceHandle
    {
    private:
        TResource _resource;
        LibraryHandle _library;

    public:
        ResourceHandle() = default;

        explicit ResourceHandle(LibraryHandle library, TResource resource):
                _library(std::move(library)),
                _resource(resource)
        {}

        ResourceRef<TResource> lock()
        {
            if(_library.isStatic())
            {
                return ResourceRef<TResource>{{}, _resource};
            }

            auto library = _library.lock();
            return ResourceRef<TResource>{ library && library->isLoaded() ? library : nullptr, _resource };
        }
    };

    template<typename TResource>
    std::shared_ptr<ResourceHandle<TResource>> makeResourceHandle(LibraryHandle& libraryHandle, TResource resource)
    {
        return std::make_shared<ResourceHandle<TResource>>(libraryHandle, resource);
    }
}
