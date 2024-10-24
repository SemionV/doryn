#pragma once

#include <memory>
#include <map>
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

        auto _get(TIdentifier identifier)
        {
            if(_serviceHandles.contains(identifier))
            {
                return _serviceHandles[identifier].lock();
            }

            return extension::ResourceRef<ServicePtrType>{{}, nullptr};
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
            return this->RegistrationEntry<TService, TIdentifierDefault>::_get();;
        }

        template<typename TService, auto identifier>
        auto get()
        {
            return this->RegistrationEntry<TService, decltype(identifier)>::_get(identifier);;
        }

        template<typename TService, typename TIdentifier>
        auto get(TIdentifier identifier)
        {
            return this->RegistrationEntry<TService, TIdentifier>::_get(identifier);;
        }
    };
}
