#pragma once

#include <memory>

namespace dory::core
{
    class Registry;

    namespace resources
    {
        enum class ServiceIdentifier;
    }

    namespace extensionPlatform
    {
        template<typename T>
        class RegistryResourceScopeRoot
        {
        protected:
            Registry& _registry;
            std::shared_ptr<T> _resource;

            RegistryResourceScopeRoot(Registry& registry, std::shared_ptr<T> resource):
                    _registry(registry),
                    _resource(resource)
            {}

        public:
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

        template<typename T, typename TIdentifier = resources::ServiceIdentifier>
        class RegistryResourceScope;

        template<typename T, typename TIdentifier>
        class RegistryResourceScope: public RegistryResourceScopeRoot<T>
        {
            TIdentifier _identifier;

        public:
            RegistryResourceScope(Registry& registry, std::shared_ptr<T> resource, TIdentifier identifier):
                    RegistryResourceScopeRoot<T>(registry, resource),
                    _identifier(identifier)
            {}

            ~RegistryResourceScope()
            {
                auto resourceRef = this->_registry.template get<T>(_identifier);
                if(resourceRef == this->_resource)
                {
                    this->_registry.template reset<T>(_identifier);
                }
            }
        };

        template<typename T>
        class RegistryResourceScope<T, resources::ServiceIdentifier>: public RegistryResourceScopeRoot<T>
        {
        public:
            RegistryResourceScope(Registry& registry, std::shared_ptr<T> resource):
                    RegistryResourceScopeRoot<T>(registry, resource)
            {}

            ~RegistryResourceScope()
            {
                auto resourceRef = this->_registry.template get<T>();
                if(resourceRef == this->_resource)
                {
                    this->_registry.template reset<T>();
                }
            }
        };
    }
}
