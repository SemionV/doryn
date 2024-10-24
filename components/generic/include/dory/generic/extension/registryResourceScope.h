#pragma once

#include <memory>

namespace dory::generic::extension
{
    template<typename TService, typename TRegistry, typename TIdentifierDefault>
    struct RegistryResourceScopePolicy
    {
        using ServiceType = TService;
        using RegistryType = TRegistry;
        using IdentifierDefaultType = TIdentifierDefault;
    };

    template<typename TPolicy>
    class RegistryResourceScopeRoot
    {
    protected:
        typename TPolicy::RegistryType& _registry;
        std::shared_ptr<typename TPolicy::ServiceType> _resource;

        RegistryResourceScopeRoot(typename TPolicy::RegistryType& registry, std::shared_ptr<typename TPolicy::ServiceType> resource):
                _registry(registry),
                _resource(resource)
        {}

    public:
        std::shared_ptr<typename TPolicy::ServiceType>& get()
        {
            return _resource;
        }

        typename TPolicy::ServiceType* operator ->()
        {
            return _resource.get();
        }

        typename TPolicy::ServiceType& operator *()
        {
            return *_resource;
        }
    };

    template<typename TPolicy, typename TIdentifier = typename TPolicy::IdentifierDefaultType>
    class RegistryResourceScope;

    template<typename TPolicy, typename TIdentifier>
    class RegistryResourceScope: public RegistryResourceScopeRoot<TPolicy>
    {
        TIdentifier _identifier;

    public:
        RegistryResourceScope(typename TPolicy::RegistryType& registry, std::shared_ptr<typename TPolicy::ServiceType> resource, TIdentifier identifier):
                RegistryResourceScopeRoot<TPolicy>(registry, resource),
                _identifier(identifier)
        {}

        ~RegistryResourceScope()
        {
            auto resourceRef = this->_registry.template get<typename TPolicy::ServiceType>(_identifier);
            if(resourceRef == this->_resource)
            {
                this->_registry.template reset<typename TPolicy::ServiceType>(_identifier);
            }
        }
    };

    template<typename TPolicy>
    class RegistryResourceScope<TPolicy, typename TPolicy::IdentifierDefaultType>: public RegistryResourceScopeRoot<TPolicy>
    {
    public:
        RegistryResourceScope(typename TPolicy::RegistryType& registry, std::shared_ptr<typename TPolicy::ServiceType> resource):
                RegistryResourceScopeRoot<TPolicy>(registry, resource)
        {}

        ~RegistryResourceScope()
        {
            auto resourceRef = this->_registry.template get<typename TPolicy::ServiceType>();
            if(resourceRef == this->_resource)
            {
                this->_registry.template reset<typename TPolicy::ServiceType>();
            }
        }
    };
}
