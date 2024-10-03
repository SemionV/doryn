#pragma once

#include "resourceRef.h"
#include "libraryHandle.h"

namespace dory::core::extensionPlatform
{
    template<typename TResource>
    class IResourceHandle: public NonCopyable
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
            auto library = _library.lock();
            return ResourceRef<TResource>{ library && library->isLoaded() ? library : nullptr, _resource };
        }
    };

    template<typename TResource>
    class StaticResourceHandle: public IResourceHandle<TResource>
    {
    private:
        TResource _resource;

    public:
        template<typename... Ts>
        explicit StaticResourceHandle(Ts&&... args):
                _resource{std::forward<Ts>(args)...}
        {}

        inline ResourceRef<TResource> lock() final
        {
            return ResourceRef<TResource>{{}, _resource };
        }
    };

    template<typename TResource>
    std::shared_ptr<IResourceHandle<TResource>> makeResourceHandle(LibraryHandle& libraryHandle, TResource resource)
    {
        return std::make_shared<dory::core::extensionPlatform::ResourceHandle<TResource>>(libraryHandle, resource);
    }

    template<typename TResource, typename... Ts>
    std::shared_ptr<IResourceHandle<TResource>> makeResourceHandle(Ts&&... resourceParameters)
    {
        return std::make_shared<StaticResourceHandle<TResource>>(std::forward<Ts>(resourceParameters)...);
    }
}
