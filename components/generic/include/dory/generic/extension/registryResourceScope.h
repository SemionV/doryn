#pragma once

#include <memory>
#include <dory/generic/registryLayer.h>

namespace dory::generic::extension
{
    template<typename TService, typename TRegistry>
    class RegistryResourceScopeRoot
    {
    protected:
        TRegistry& _registry;
        std::shared_ptr<TService> _resource;

        RegistryResourceScopeRoot(TRegistry& registry, std::shared_ptr<TService> resource):
                _registry(registry),
                _resource(resource)
        {}

    public:
        std::shared_ptr<TService>& get()
        {
            return _resource;
        }

        TService* operator ->()
        {
            return _resource.get();
        }

        TService& operator *()
        {
            return *_resource;
        }
    };

    template<typename TService, typename TRegistry, typename TIdentifier = dory::generic::registry::ServiceIdentifier>
    class RegistryResourceScope;

    template<typename TService, typename TRegistry, typename TIdentifier>
    class RegistryResourceScope: public RegistryResourceScopeRoot<TService, TRegistry>
    {
        TIdentifier _identifier;

    public:
        RegistryResourceScope(TRegistry& registry, std::shared_ptr<TService> resource, TIdentifier identifier):
                RegistryResourceScopeRoot<TService, TRegistry>(registry, resource),
                _identifier(identifier)
        {}

        ~RegistryResourceScope()
        {
            auto resourceRef = this->_registry.template get<TService>(_identifier);
            if(resourceRef == this->_resource)
            {
                this->_registry.template reset<TService>(_identifier);
            }
        }
    };

    template<typename TService, typename TRegistry>
    class RegistryResourceScope<TService, TRegistry, registry::ServiceIdentifier>: public RegistryResourceScopeRoot<TService, TRegistry>
    {
    public:
        RegistryResourceScope(TRegistry& registry, std::shared_ptr<TService> resource):
                RegistryResourceScopeRoot<TService, TRegistry>(registry, resource)
        {}

        ~RegistryResourceScope()
        {
            auto resourceRef = this->_registry.template get<TService>();
            if(resourceRef == this->_resource)
            {
                this->_registry.template reset<TService>();
            }
        }
    };
}
