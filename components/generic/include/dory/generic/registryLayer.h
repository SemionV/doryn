#pragma once

#include <memory>
#include <map>
#include <type_traits>
#include "extension/resourceHandle.h"

namespace dory::generic::registry
{
    template<typename TServiceInterface, typename TIdentifier>
    class RegistrationEntry
    {
    private:
        using ServicePtrType = std::shared_ptr<TServiceInterface>;
        std::optional<extension::ResourceHandle<ServicePtrType>> _serviceHandle;
        std::map<TIdentifier, extension::ResourceHandle<ServicePtrType>> _serviceHandles;

        template<typename T, typename A>
        void invoke(extension::ResourceRef<T>& resourceRef, const A& action)
        {
            if(resourceRef.operator bool())
            {
                action((*resourceRef).get());
            }
        }

    protected:
        void _set(extension::LibraryHandle libraryHandle, std::shared_ptr<TServiceInterface> service)
        {
            _serviceHandle = extension::ResourceHandle(libraryHandle, std::move(service));
        }

        void _set(extension::LibraryHandle libraryHandle, std::shared_ptr<TServiceInterface> service, TIdentifier identifier)
        {
            _serviceHandles.emplace(identifier, extension::ResourceHandle(libraryHandle, std::move(service)));
        }

        void _reset()
        {
            _serviceHandle = {};
        }

        void _reset(TIdentifier identifier)
        {
            if(_serviceHandles.contains(identifier))
            {
                _serviceHandles.erase(identifier);
            }
        }

        auto _get()
        {
            if(_serviceHandle)
            {
                return _serviceHandle->lock();
            }

            return extension::ResourceRef<ServicePtrType>{{}, nullptr};
        }

        auto _get(const TIdentifier& identifier)
        {
            if(_serviceHandles.contains(identifier))
            {
                return _serviceHandles[identifier].lock();
            }

            return extension::ResourceRef<ServicePtrType>{{}, nullptr};
        }

        template<typename A>
        requires(std::is_invocable_v<A, TServiceInterface*>)
        void _get(A&& action)
        {
            auto resourceRef = _get();
            invoke(resourceRef, std::forward<A>(action));
        }

        template<typename A>
        void _get(TIdentifier identifier, A&& action)
        {
            auto resourceRef = _get(identifier);
            invoke(resourceRef, std::forward<A>(action));
        }

    };

    template<typename TIdentifierDefault, typename... TServices>
    struct RegistryLayer: public RegistrationEntry<typename TServices::InterfaceType, typename TServices::IdentifierType>...
    {
    public:
        template<typename TInterface>
        void set(extension::LibraryHandle libraryHandle, std::shared_ptr<TInterface> service)
        {
            this->RegistrationEntry<TInterface, TIdentifierDefault>::_set(libraryHandle, service);
        }

        template<typename TInterface, auto identifier>
        void set(extension::LibraryHandle libraryHandle, std::shared_ptr<TInterface> service)
        {
            this->RegistrationEntry<TInterface, decltype(identifier)>::_set(libraryHandle, service, identifier);
        }

        template<typename TInterface, typename TIdentifier>
        void set(extension::LibraryHandle libraryHandle, std::shared_ptr<TInterface> service, TIdentifier identifier)
        {
            this->RegistrationEntry<TInterface, TIdentifier>::_set(libraryHandle, service, identifier);
        }

        template<typename TService>
        void reset()
        {
            this->RegistrationEntry<TService, TIdentifierDefault>::_reset();
        }

        template<typename TService, auto identifier>
        void reset()
        {
            this->RegistrationEntry<TService, decltype(identifier)>::_reset(identifier);
        }

        template<typename TService, typename TIdentifier>
        void reset(TIdentifier identifier)
        {
            this->RegistrationEntry<TService, TIdentifier>::_reset(identifier);
        }

        template<typename TService>
        auto get()
        {
            return this->RegistrationEntry<TService, TIdentifierDefault>::_get();
        }

        template<typename TService, auto identifier>
        auto get()
        {
            return this->RegistrationEntry<TService, decltype(identifier)>::_get(identifier);
        }

        template<typename TService, typename TIdentifier>
        requires(!std::is_invocable_v<TIdentifier, TService*>)
        auto get(TIdentifier&& identifier)
        {
            return this->RegistrationEntry<TService, TIdentifier>::_get(std::forward<TIdentifier>(identifier));
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        template<typename TService, typename A>
        requires(std::is_invocable_v<A, TService*>)
        void get(A&& action)
        {
            this->RegistrationEntry<TService, TIdentifierDefault>::_get(std::forward<A>(action));
        }

        template<typename TService, auto identifier, typename A>
        void get(A&& action)
        {
            this->RegistrationEntry<TService, decltype(identifier)>::_get(identifier, std::forward<A>(action));
        }

        template<typename TService, typename TIdentifier, typename A>
        void get(TIdentifier identifier, A&& action)
        {
            this->RegistrationEntry<TService, TIdentifier>::_get(identifier, std::forward<A>(action));
        }
    };
}
