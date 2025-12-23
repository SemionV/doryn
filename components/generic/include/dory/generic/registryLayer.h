#pragma once

#include <memory>
#include <map>
#include <type_traits>
    #include "extension/resourceHandle.h"

namespace dory::generic::registry
{
    enum class ServiceIdentifier
    {
        Default
    };

    template<typename TInterface, typename TIdentifier = ServiceIdentifier>
    struct ServiceEntry
    {
        using InterfaceType = TInterface;
        using IdentifierType = TIdentifier;
    };

    template<typename TInterface, auto Identifier = ServiceIdentifier::Default>
    struct Service
    {
        using InterfaceType = TInterface;
        static const constexpr decltype(Identifier) identifier = Identifier;
    };

    template<typename TServiceInterface>
    class RegistrationEntryRoot
    {
    public:
        using ServicePtrType = std::shared_ptr<TServiceInterface>;

    protected:
        template<typename A>
        static void invoke(extension::ResourceRef<ServicePtrType>& resourceRef, const A& action)
        {
            if(resourceRef.operator bool())
            {
                action((*resourceRef).get());
            }
        }
    };

    template<typename TServiceInterface, typename TIdentifier>
    class RegistrationEntry: RegistrationEntryRoot<TServiceInterface>
    {
    private:
        using HandlesMapType = std::map<TIdentifier, extension::ResourceHandle<typename RegistrationEntryRoot<TServiceInterface>::ServicePtrType>>;
        HandlesMapType _serviceHandles;

    protected:
        void _set(extension::LibraryHandle libraryHandle, std::shared_ptr<TServiceInterface> service, TIdentifier identifier)
        {
            _serviceHandles[identifier] = extension::ResourceHandle(libraryHandle, std::move(service));
        }

        void _reset(TIdentifier identifier)
        {
            if(_serviceHandles.contains(identifier))
            {
                _serviceHandles[identifier] = {};
            }
        }

        auto _get(const TIdentifier& identifier)
        {
            if(_serviceHandles.contains(identifier))
            {
                return _serviceHandles[identifier].lock();
            }

            return extension::ResourceRef<typename RegistrationEntryRoot<TServiceInterface>::ServicePtrType>{{}, nullptr};
        }

        template<typename A>
        void _get(TIdentifier identifier, A&& action)
        {
            auto resourceRef = _get(identifier);
            RegistrationEntryRoot<TServiceInterface>::invoke(resourceRef, std::forward<A>(action));
        }

        auto _getHandle(const TIdentifier& identifier)
        {
            if(_serviceHandles.contains(identifier))
            {
                return _serviceHandles[identifier];
            }

            return extension::ResourceHandle<typename RegistrationEntryRoot<TServiceInterface>::ServicePtrType>{extension::LibraryHandle{}, nullptr};
        }

        template<typename A>
        requires(std::is_invocable_v<A, const HandlesMapType&>)
        void _getAll(A&& action)
        {
            action(_serviceHandles);
        }
    };

    template<typename TServiceInterface>
    class RegistrationEntry<TServiceInterface, ServiceIdentifier>: RegistrationEntryRoot<TServiceInterface>
    {
    private:
        std::optional<extension::ResourceHandle<typename RegistrationEntryRoot<TServiceInterface>::ServicePtrType>> _serviceHandle;

    protected:
        void _set(extension::LibraryHandle libraryHandle, std::shared_ptr<TServiceInterface> service)
        {
            _serviceHandle = extension::ResourceHandle(libraryHandle, std::move(service));
        }

        void _reset()
        {
            _serviceHandle = {};
        }

        auto _get()
        {
            if(_serviceHandle)
            {
                return _serviceHandle->lock();
            }

            return extension::ResourceRef<typename RegistrationEntryRoot<TServiceInterface>::ServicePtrType>{{}, nullptr};
        }

        auto _get(const ServiceIdentifier& identifier)
        {
            return _get();
        }

        auto _getHandle()
        {
            return _serviceHandle;
        }

        template<typename A>
        requires(std::is_invocable_v<A, TServiceInterface*>)
        void _get(A&& action)
        {
            auto resourceRef = _get();
            RegistrationEntryRoot<TServiceInterface>::invoke(resourceRef, std::forward<A>(action));
        }
    };

    template<typename... TServices>
    struct RegistryLayer: public RegistrationEntry<typename TServices::InterfaceType, typename TServices::IdentifierType>...
    {
    private:
        template<typename TAction, typename... TService>
        void invoke(TAction&& action, generic::extension::ResourceRef<std::shared_ptr<TService>>... serviceRefs)
        {
            if((serviceRefs && ...))
            {
                action((*serviceRefs).get()...);
            }
        }

    public:
        template<typename TInterface>
        void set(extension::LibraryHandle libraryHandle, std::shared_ptr<TInterface> service)
        {
            this->RegistrationEntry<TInterface, ServiceIdentifier>::_set(libraryHandle, service);
        }

        template<typename TInterface, auto identifier>
        void set(extension::LibraryHandle libraryHandle, std::shared_ptr<TInterface> service)
        {
            this->RegistrationEntry<TInterface, decltype(identifier)>::_set(libraryHandle, service, identifier);
        }

        template<typename TInterface, typename TIdentifier>
        void set(extension::LibraryHandle libraryHandle, std::shared_ptr<TInterface> service, TIdentifier identifier)
        {
            this->RegistrationEntry<TInterface, std::decay_t<TIdentifier>>::_set(libraryHandle, service, identifier);
        }

        template<typename TService>
        void reset()
        {
            this->RegistrationEntry<TService, ServiceIdentifier>::_reset();
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
            return this->RegistrationEntry<TService, ServiceIdentifier>::_get();
        }

        template<typename TService>
        auto getHandle()
        {
            return this->RegistrationEntry<TService, ServiceIdentifier>::_getHandle();
        }

        template<typename TService, auto identifier>
        auto getHandle()
        {
            return this->RegistrationEntry<TService, decltype(identifier)>::_getHandle(identifier);
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
            return this->RegistrationEntry<TService, std::decay_t<TIdentifier>>::_get(std::forward<TIdentifier>(identifier));
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        template<typename TService, typename A>
        requires(std::is_invocable_v<A, TService*>)
        void get(A&& action)
        {
            this->RegistrationEntry<TService, ServiceIdentifier>::_get(std::forward<A>(action));
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

        template<typename... TServiceEntry, typename A>
        requires(std::is_invocable_v<A, typename TServiceEntry::InterfaceType*...>)
        void get(A&& action)
        {
            invoke(std::forward<A>(action), this->RegistrationEntry<typename TServiceEntry::InterfaceType, std::decay_t<decltype(TServiceEntry::identifier)>>::_get(TServiceEntry::identifier)...);
        }

        template<typename TService, typename TIdentifier, typename A>
        void getAll(A&& action)
        {
            this->RegistrationEntry<TService, TIdentifier>::_getAll(std::forward<A>(action));
        }
    };
}
