#pragma once

#include "resourceRef.h"
#include "libraryHandle.h"

namespace dory::core::extensionPlatform
{
    template<typename TResource>
    class IResourceHandle
    {
    public:
        virtual ~IResourceHandle() = default;

        virtual ResourceRef<TResource> lock() = 0;
    };

    template<typename TResource>
    class ResourceHandle: public IResourceHandle<TResource>
    {
    private:
        TResource _resource;
        LibraryHandle _library;

    public:
        explicit ResourceHandle(LibraryHandle library, TResource resource):
                _library(std::move(library)),
                _resource(resource)
        {}

        inline ResourceRef<TResource> lock() final
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
    std::shared_ptr<IResourceHandle<TResource>> makeResourceHandle(LibraryHandle& libraryHandle, TResource resource)
    {
        return std::make_shared<dory::core::extensionPlatform::ResourceHandle<TResource>>(libraryHandle, resource);
    }
}
