#pragma once

#include "iLibrary.h"
#include "resourceRef.h"
#include "libraryHandle.h"

namespace dory::generic::extension
{
    template<typename TResource>
    class ResourceHandle
    {
    protected:
        TResource _resource;
        LibraryHandle _library;

    public:
        ResourceHandle() = default;

        explicit ResourceHandle(const LibraryHandle& library, TResource resource):
                _library(library),
                _resource(resource)
        {}

        template<typename TResourceParentType>
        ResourceHandle<TResourceParentType> clone()
        {
            return ResourceHandle<TResourceParentType>{ _library, _resource };
        }

        ResourceRef<TResource> lock() const
        {
            if(_library.isStatic())
            {
                return ResourceRef<TResource>{{}, _resource};
            }

            auto library = _library.lock();
            return ResourceRef<TResource>{ library && library->isLoaded() ? library : nullptr, _resource };
        }
    };

    template<typename TInterface>
    class ResourceHandle<std::shared_ptr<TInterface>>
    {
    private:
        std::shared_ptr<TInterface> _resource;
        LibraryHandle _library;

    public:
        ResourceHandle() = default;

        explicit ResourceHandle(const LibraryHandle& library, std::shared_ptr<TInterface> resource):
                _library(library),
                _resource(resource)
        {}

        template<typename TResourceParentType>
        ResourceHandle<TResourceParentType> clone()
        {
            return ResourceHandle<TResourceParentType>{ _library, _resource };
        }

        ResourceRef<std::shared_ptr<TInterface>> lock() const
        {
            if(_library.isStatic())
            {
                return ResourceRef<std::shared_ptr<TInterface>>{{}, _resource};
            }

            auto library = _library.lock();
            return ResourceRef<std::shared_ptr<TInterface>>{ library && library->isLoaded() ? library : nullptr, _resource };
        }

        template<typename TCast>
        ResourceRef<std::shared_ptr<TCast>> lock() const
        {
            if(_library.isStatic())
            {
                return ResourceRef<std::shared_ptr<TCast>>{{}, std::static_pointer_cast<TCast>(_resource)};
            }

            auto library = _library.lock();
            return ResourceRef<std::shared_ptr<TCast>>{ library && library->isLoaded() ? library : nullptr, std::static_pointer_cast<TCast>(_resource) };
        }
    };

    template<typename TResource>
    auto makeResourceHandle(LibraryHandle& libraryHandle, TResource resource)
    {
        return std::make_shared<ResourceHandle<TResource>>(libraryHandle, resource);
    }
}
