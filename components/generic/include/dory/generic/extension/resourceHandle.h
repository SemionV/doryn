#pragma once

#include <utility>

#include "iLibrary.h"
#include "resourceRef.h"
#include "libraryHandle.h"

namespace dory::generic::extension
{
    template<typename TResource, template<class> class TImplementation>
    class ResourceHandleRoot
    {
    protected:
        TResource _resource;
        LibraryHandle _library;

    public:
        ResourceHandleRoot() = default;

        explicit ResourceHandleRoot(LibraryHandle  library, TResource resource):
                _resource(resource),
                _library(std::move(library))
        {}

        template<typename TResourceParentType>
        TImplementation<TResourceParentType> clone()
        {
            return TImplementation<TResourceParentType>{ _library, _resource };
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

    template<typename TResource>
    class ResourceHandle: public ResourceHandleRoot<TResource, ResourceHandle>
    {
    public:
        ResourceHandle() = default;

        explicit ResourceHandle(const LibraryHandle& library, TResource resource):
        ResourceHandleRoot<TResource, ResourceHandle>(library, resource)
        {}
    };

    template<typename TInterface>
    class ResourceHandle<std::shared_ptr<TInterface>>: public ResourceHandleRoot<std::shared_ptr<TInterface>, ResourceHandle>
    {
    public:
        ResourceHandle() = default;

        explicit ResourceHandle(const LibraryHandle& library, std::shared_ptr<TInterface> resource):
                ResourceHandleRoot<std::shared_ptr<TInterface>, ResourceHandle>(library, resource)
        {}

        ResourceRef<std::shared_ptr<TInterface>> lock() const
        {
            return ResourceHandleRoot<std::shared_ptr<TInterface>, ResourceHandle>::lock();
        }

        template<typename TCast>
        ResourceRef<std::shared_ptr<TCast>> lock() const
        {
            if(this->_library.isStatic())
            {
                return ResourceRef<std::shared_ptr<TCast>>{{}, std::static_pointer_cast<TCast>(this->_resource)};
            }

            auto library = this->_library.lock();
            return ResourceRef<std::shared_ptr<TCast>>{ library && library->isLoaded() ? library : nullptr, std::static_pointer_cast<TCast>(this->_resource) };
        }
    };

    template<typename TResource>
    auto makeResourceHandle(LibraryHandle& libraryHandle, TResource resource)
    {
        return std::make_shared<ResourceHandle<TResource>>(libraryHandle, resource);
    }
}
