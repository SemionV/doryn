#pragma once

#include "dory/core/iSetup.h"
#include "dory/generic/macros.h"
#include "dory/core/repository.h"
#include "dory/core/events.h"
#include "dory/core/services/objectFactory.h"
#include <dory/core/allocators.h>

namespace dory::game
{
    template<typename TAllocator>
    class SegregationResource final : public std::pmr::memory_resource
    {
    public:
        explicit SegregationResource(TAllocator& allocator):
            _allocator(&allocator)
        {}

    private:
        TAllocator* _allocator;

        void* do_allocate(const std::size_t bytes, std::size_t alignment) override
        {
            return _allocator->allocate(bytes);
        }

        void do_deallocate(void* p, std::size_t bytes, std::size_t alignment) override
        {
            _allocator->deallocate(p, bytes);
        }

        [[nodiscard]] bool do_is_equal(const std::pmr::memory_resource& other) const noexcept override
        {
            return this == &other;
        }
    };

    class DORY_DLL_API Setup: public core::ISetup
    {
    private:
        using AllocatorType = core::GlobalAllocatorType;
        AllocatorType& _globalAllocator;

        template<typename T>
        using StandardAllocatorType = memory::StandardAllocator<T, AllocatorType>;

        SegregationResource<AllocatorType> _globalResource{ _globalAllocator };

    private:
        template<typename TEventBundle>
        void registerEventBundle(const generic::extension::LibraryHandle& libraryHandle, core::Registry& registry)
        {
            using DispatcherType = core::events::DispatcherCannon<typename TEventBundle::IListener,
                    typename TEventBundle::IDispatcher,
                    typename TEventBundle::EventListType>;

            std::pmr::polymorphic_allocator<DispatcherType> pa{ &_globalResource };
            auto instance = std::allocate_shared<DispatcherType>(pa);

            registry.set<typename TEventBundle::IDispatcher>(libraryHandle, instance);
            registry.set<typename TEventBundle::IListener>(libraryHandle, instance);
        }

        template<typename TEventBundle>
        void registerEventBufferBundle(const generic::extension::LibraryHandle& libraryHandle, core::Registry& registry)
        {
            using DispatcherType = core::events::DispatcherCannonBuffer<typename TEventBundle::IListener,
                    typename TEventBundle::IDispatcher,
                    typename TEventBundle::EventListType>;

            std::pmr::polymorphic_allocator<DispatcherType> pa{ &_globalResource };
            auto instance = std::allocate_shared<DispatcherType>(pa);

            registry.set<typename TEventBundle::IDispatcher>(libraryHandle, instance);
            registry.set<typename TEventBundle::IListener>(libraryHandle, instance);
        }

        template<typename TEntity>
        void registerRepository(const generic::extension::LibraryHandle& libraryHandle, core::Registry& registry)
        {
            std::pmr::polymorphic_allocator<core::repositories::Repository<TEntity>> pa{ &_globalResource };
            auto instance = std::allocate_shared<core::repositories::Repository<TEntity>>(pa);

            registry.set<core::repositories::IRepository<TEntity>>(libraryHandle, instance);
        }

        template<typename TInterface, typename TImplementation>
        void registerObjectFactory(const char* name, const generic::extension::LibraryHandle& libraryHandle, core::Registry& registry)
        {
            std::pmr::polymorphic_allocator<core::services::ObjectFactory<TInterface, TImplementation>> pa{ &_globalResource };
            auto factory = std::allocate_shared<core::services::ObjectFactory<TInterface, TImplementation>>(pa, libraryHandle, registry);

            registry.set<core::services::IObjectFactory<TInterface>>(libraryHandle, factory, core::resources::Name{ name });
        }

        template<typename TInterface, typename TInstanceInterface>
        void registerSingletonObjectFactory(const char* name, const generic::extension::LibraryHandle& libraryHandle, core::Registry& registry)
        {
            std::pmr::polymorphic_allocator<core::services::SingletonObjectFactory<TInterface, TInstanceInterface>> pa{ &_globalResource };
            auto factory = std::allocate_shared<core::services::SingletonObjectFactory<TInterface, TInstanceInterface>>(pa, registry);

            registry.set<core::services::IObjectFactory<TInterface>>(libraryHandle, factory, core::resources::Name{ name });
        }

        template<typename TInterface, typename TInstanceInterface, auto Identifier>
        void registerSingletonObjectFactory(const char* name, const generic::extension::LibraryHandle& libraryHandle, core::Registry& registry)
        {
            using ObjectFactoryType = core::services::SingletonIdentifierObjectFactory<TInterface, TInstanceInterface, Identifier>;
            std::pmr::polymorphic_allocator<ObjectFactoryType> pa{ &_globalResource };
            auto factory = std::allocate_shared<ObjectFactoryType>(pa, registry);

            registry.set<core::services::IObjectFactory<TInterface>>(libraryHandle, factory, core::resources::Name{ name });
        }

    public:
        explicit Setup(core::GlobalAllocatorType& globalAllocator):
            _globalAllocator(globalAllocator)
        {}

        void setupRegistry(const generic::extension::LibraryHandle& libraryHandle, core::Registry& registry,
                            const core::resources::configuration::Configuration& configuration) override;
    };
}