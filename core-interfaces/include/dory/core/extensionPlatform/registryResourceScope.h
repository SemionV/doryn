#pragma once

#include <dory/core/registry.h>
#include <memory>

namespace dory::core::extensionPlatform
{
    template<typename T>
    class RegistryResourceScope
    {
    public:
        Registry& _registry;
        std::shared_ptr<T> _resource;

    public:
        RegistryResourceScope(Registry& registry, std::shared_ptr<T> resource):
                _registry(registry),
                _resource(resource)
        {}

        ~RegistryResourceScope()
        {
            if(_resource)
            {
                auto resourceRef = _registry.get<T>();
                if(resourceRef == _resource)
                {
                    _registry.reset<T>();
                }
            }
        }

        std::shared_ptr<T>& get()
        {
            return _resource;
        }

        T* operator ->()
        {
            return _resource.get();
        }

        T& operator *()
        {
            return *_resource;
        }

    };
}
