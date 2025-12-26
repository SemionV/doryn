#pragma once

#include <dory/generic/typeList.h>
#include <dory/generic/extension/libraryHandle.h>
#include <dory/generic/extension/resourceHandle.h>

namespace dory::generic::registry
{
    enum class ServiceIdentifier
    {
        Default
    };

    template<typename TServiceInterface, auto Identifier>
    struct ServiceQuery
    {
        using InterfaceType = TServiceInterface;
        static constexpr decltype(Identifier) identifier = Identifier;
    };

    template<typename TServiceInterface, typename TIdentifierList = generic::ValueList<ServiceIdentifier, ServiceIdentifier::Default>>
    struct ServiceEntry
    {
        using InterfaceType = TServiceInterface;
        using IdentifierListType = TIdentifierList;
    };

    template<typename... TServices>
    struct ServiceCountTraverse;

    template<typename TService, typename... TServices>
    struct ServiceCountTraverse<TService, TServices...>
    {
        static constexpr std::size_t value = generic::ValueListLength<typename TService::IdentifierListType>::value + ServiceCountTraverse<TServices...>::value;
    };

    template<>
    struct ServiceCountTraverse<>
    {
        static constexpr std::size_t value = 0;
    };

    template<typename TServiceList>
    struct ServiceCount;

    template<typename... TServices>
    struct ServiceCount<generic::TypeList<TServices...>>
    {
        static constexpr std::size_t value = ServiceCountTraverse<TServices...>::value;
    };

    template<std::size_t Index, typename TServiceInterface, typename... TServices>
    struct ServiceIndexTraverse;

    template<std::size_t Index, typename TServiceInterface, typename TService, typename... TServices>
    struct ServiceIndexTraverse<Index, TServiceInterface, TService, TServices...>
    {
        using NextServiceTraverseType = ServiceIndexTraverse<Index + generic::ValueListLength<typename TService::IdentifierListType>::value, TServiceInterface, TServices...>;
        using ServiceEntryType = std::conditional_t<std::is_same_v<TServiceInterface, typename TService::InterfaceType>, TService, typename NextServiceTraverseType::ServiceEntryType>;
        static constexpr int value = std::is_same_v<TServiceInterface, typename TService::InterfaceType> ? Index : NextServiceTraverseType::value;
    };

    template<std::size_t Index, typename TServiceInterface>
    struct ServiceIndexTraverse<Index, TServiceInterface>
    {
        using ServiceEntryType = ServiceEntry<void>;
        static constexpr int value = -1;
    };

    template<typename TServiceInterface, auto Identifier, typename TServiceList>
    struct ServiceIndex;

    template<typename TServiceInterface, auto Identifier, typename... TServices>
    struct ServiceIndex<TServiceInterface, Identifier, generic::TypeList<TServices...>>
    {
        using IndexTraverseType = ServiceIndexTraverse<0, TServiceInterface, TServices...>;
        static constexpr int offset = generic::ValueIndex<Identifier, typename IndexTraverseType::ServiceEntryType::IdentifierListType>::value;
        static_assert(offset >= 0); //Service Identifier is not registered
        static constexpr int value = IndexTraverseType::value + offset;
        static_assert(value >= 0); // Service Interface is not registered
    };

    template<typename TServiceInterface, typename TServiceList>
    struct ServiceIndexRuntime;

    template<typename TServiceInterface, typename... TServices>
    struct ServiceIndexRuntime<TServiceInterface, generic::TypeList<TServices...>>
    {
        using IndexTraverseType = ServiceIndexTraverse<0, TServiceInterface, TServices...>;
        static constexpr int value = IndexTraverseType::value;
        static_assert(value >= 0); // Service Interface is not registered

        static int get(IndexTraverseType::ServiceEntryType::IdentifierListType::ValueType searchValue)
        {
            return value + generic::ValueIndexRuntime<typename IndexTraverseType::ServiceEntryType::IdentifierListType>::get(searchValue);
        }
    };

    template<typename TServiceInterface, typename TServiceList>
    struct ServiceIndexAnchor;

    template<typename TServiceInterface, typename... TServices>
    struct ServiceIndexAnchor<TServiceInterface, generic::TypeList<TServices...>>
    {
        using IndexTraverseType = ServiceIndexTraverse<0, TServiceInterface, TServices...>;
        using ServiceEntryType = IndexTraverseType::ServiceEntryType;
        using ServiceIdentifierType = ServiceEntryType::IdentifierListType::ValueType;
        static constexpr int value = IndexTraverseType::value;
        static_assert(value >= 0); // Service Interface is not registered
    };

    template<typename TServiceList>
    class RegistryLayer
    {
    private:
        using BaseInterfaceType = void;
        template<typename TInterface>
        using ServicePointerType = std::shared_ptr<TInterface>;
        using ServiceHandleType = generic::extension::ResourceHandle<ServicePointerType<BaseInterfaceType>>;
        using StorageEntryType = std::optional<ServiceHandleType>;
        std::array<StorageEntryType, ServiceCount<TServiceList>::value> _services;

    private:
        template<typename TServiceInterface, auto Identifier = ServiceIdentifier::Default>
        constexpr int getServiceEntryIndex()
        {
            constexpr auto index = ServiceIndex<TServiceInterface, Identifier, TServiceList>::value;
            static_assert(index >= 0 && index < generic::Length<TServiceList>::value); //Index is out of range

            return index;
        }

        template<typename TServiceInterface, typename TIdentifier>
        constexpr int getServiceEntryIndex(TIdentifier identifier)
        {
            const auto index = ServiceIndexRuntime<TServiceInterface, TServiceList>::get(identifier);
            assert::debug(index >= 0 && index < generic::Length<TServiceList>::value, "Index is out of range");

            return index;
        }

        void set_internal(std::size_t index, const generic::extension::LibraryHandle& libraryHandle, const ServicePointerType<BaseInterfaceType>& service)
        {
            _services[index] = generic::extension::ResourceHandle{ libraryHandle, std::static_pointer_cast<BaseInterfaceType>(service) };
        }

        template<typename TServiceInterface>
        auto get_internal(std::size_t index)
        {
            if(auto entry = _services[index])
            {
                return entry->template lock<TServiceInterface>();
            }

            return dory::generic::extension::ResourceRef<ServicePointerType<TServiceInterface>>{ {}, nullptr };
        }

        void reset_internal(std::size_t index)
        {
            _services[index] = {};
        }

        auto& getHandle_internal(std::size_t index)
        {
            return _services[index];
        }

        template<typename A, typename TServiceInterface>
        void invoke(const std::size_t index, const A& action)
        {
            auto service = get_internal<TServiceInterface>(index);
            if(service.operator bool())
            {
                action((*service).get());
            }
        }

        template<typename TAction, typename... TServiceInterface>
        void invoke(TAction&& action, generic::extension::ResourceRef<std::shared_ptr<TServiceInterface>>... serviceRefs)
        {
            if((serviceRefs && ...))
            {
                action((*serviceRefs).get()...);
            }
        }

    public:
        std::size_t getServiceSlotCount()
        {
            return _services.size();
        }

        template<typename TServiceInterface>
        void set(const generic::extension::LibraryHandle& libraryHandle, const ServicePointerType<BaseInterfaceType>& service)
        {
            const auto index = getServiceEntryIndex<TServiceInterface>();
            set_internal(index, libraryHandle, service);
        }

        template<typename TServiceInterface, auto Identifier>
        void set(const generic::extension::LibraryHandle& libraryHandle, const ServicePointerType<BaseInterfaceType>& service)
        {
            const auto index = getServiceEntryIndex<TServiceInterface, Identifier>();
            set_internal(index, libraryHandle, service);
        }

        template<typename TServiceInterface, typename TIdentifier>
        void set(const generic::extension::LibraryHandle& libraryHandle, const ServicePointerType<BaseInterfaceType>& service, TIdentifier&& identifier)
        {
            const auto index = getServiceEntryIndex<TServiceInterface>(std::forward<TIdentifier>(identifier));
            set_internal(index, libraryHandle, service);
        }

        template<typename TServiceInterface>
        void reset()
        {
            const auto index = getServiceEntryIndex<TServiceInterface>();
            reset_internal(index);
        }

        template<typename TServiceInterface, auto Identifier>
        void reset()
        {
            const auto index = getServiceEntryIndex<TServiceInterface, Identifier>();
            reset_internal(index);
        }

        template<typename TServiceInterface, typename TIdentifier>
        void reset(TIdentifier identifier)
        {
            const auto index = getServiceEntryIndex<TServiceInterface>(identifier);
            reset_internal(index);
        }

        template<typename TServiceInterface>
        auto get()
        {
            const auto index = getServiceEntryIndex<TServiceInterface>();
            return get_internal<TServiceInterface>(index);
        }

        template<typename TServiceInterface, auto Identifier>
        auto get()
        {
            const auto index = getServiceEntryIndex<TServiceInterface, Identifier>();
            return get_internal<TServiceInterface>(index);
        }

        template<typename TServiceInterface, typename TIdentifier>
        auto get(TIdentifier&& identifier)
        {
            const auto index = getServiceEntryIndex<TServiceInterface>(std::forward<TIdentifier>(identifier));
            return get_internal<TServiceInterface>(index);
        }

        template<typename TServiceInterface, typename A>
        requires(std::is_invocable_v<A, TServiceInterface*>)
        void get(A&& action)
        {
            const auto index = getServiceEntryIndex<TServiceInterface>();
            invoke<TServiceInterface>(index, std::forward<TServiceInterface>(action));
        }

        template<typename TServiceInterface, auto Identifier, typename A>
        void get(A&& action)
        {
            const auto index = getServiceEntryIndex<TServiceInterface, Identifier>();
            invoke<TServiceInterface>(index, std::forward<TServiceInterface>(action));
        }

        template<typename TServiceInterface, typename TIdentifier, typename A>
        void get(TIdentifier identifier, A&& action)
        {
            const auto index = getServiceEntryIndex<TServiceInterface>(identifier);
            invoke<TServiceInterface>(index, std::forward<TServiceInterface>(action));
        }

        template<typename... TServiceQuery, typename A>
        requires(std::is_invocable_v<A, typename TServiceQuery::InterfaceType*...>)
        void get(A&& action)
        {
            invoke(std::forward<A>(action), get<typename TServiceQuery::InterfaceType, std::decay_t<decltype(TServiceQuery::identifier)>>()...);
        }

        template<typename TServiceInterface, typename A>
        void getAll(A&& action)
        {
            using ServiceAnchorType = ServiceIndexAnchor<TServiceInterface, TServiceList>;
            constexpr auto identifiers = generic::ValueArray<typename ServiceAnchorType::ServiceEntryType::IdentifierListType>::get();
            for(auto identifier : identifiers)
            {
                auto serviceRef = get<TServiceInterface>(identifier);
                TServiceInterface* service = (*serviceRef).get();
                action(identifier, service);
            }
        }

        template<typename TServiceInterface>
        auto getHandle()
        {
            const auto index = getServiceEntryIndex<TServiceInterface>();
            return getHandle_internal(index);
        }

        template<typename TServiceInterface, auto Identifier>
        auto getHandle()
        {
            const auto index = getServiceEntryIndex<TServiceInterface, Identifier>();
            return getHandle_internal(index);
        }

        template<typename TServiceInterface, typename TIdentifier>
        auto getHandle(TIdentifier&& identifier)
        {
            const auto index = getServiceEntryIndex<TServiceInterface>(std::forward<TIdentifier>(identifier));
            return getHandle_internal(index);
        }
    };
}