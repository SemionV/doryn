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

            auto instance = createInstance<DispatcherType>();

            registry.set<typename TEventBundle::IDispatcher>(libraryHandle, instance);
            registry.set<typename TEventBundle::IListener>(libraryHandle, instance);
        }

        template<typename TEventBundle>
        void registerEventBufferBundle(const generic::extension::LibraryHandle& libraryHandle, core::Registry& registry)
        {
            using DispatcherType = core::events::DispatcherCannonBuffer<typename TEventBundle::IListener,
                    typename TEventBundle::IDispatcher,
                    typename TEventBundle::EventListType>;

            auto instance = createInstance<DispatcherType>();

            registry.set<typename TEventBundle::IDispatcher>(libraryHandle, instance);
            registry.set<typename TEventBundle::IListener>(libraryHandle, instance);
        }

        template<typename TEntity>
        void registerRepository(const generic::extension::LibraryHandle& libraryHandle, core::Registry& registry)
        {
            auto instance = createInstance<core::repositories::Repository<TEntity>>();

            registry.set<core::repositories::IRepository<TEntity>>(libraryHandle, instance);
        }

        template<typename TInterface, typename TImplementation>
        void registerObjectFactory(const char* name, const generic::extension::LibraryHandle& libraryHandle, core::Registry& registry)
        {
            auto factory = createInstance<core::services::ObjectFactory<TInterface, TImplementation>>(libraryHandle, registry);

            registry.set<core::services::IObjectFactory<TInterface>>(libraryHandle, factory, core::resources::Name{ name });
        }

        template<typename TInterface, typename TInstanceInterface>
        void registerSingletonObjectFactory(const char* name, const generic::extension::LibraryHandle& libraryHandle, core::Registry& registry)
        {
            auto factory = createInstance<core::services::SingletonObjectFactory<TInterface, TInstanceInterface>>(registry);

            registry.set<core::services::IObjectFactory<TInterface>>(libraryHandle, factory, core::resources::Name{ name });
        }

        template<typename TInterface, typename TInstanceInterface, auto Identifier>
        void registerSingletonObjectFactory(const char* name, const generic::extension::LibraryHandle& libraryHandle, core::Registry& registry)
        {
            using ObjectFactoryType = core::services::SingletonIdentifierObjectFactory<TInterface, TInstanceInterface, Identifier>;
            auto factory = createInstance<ObjectFactoryType>(registry);

            registry.set<core::services::IObjectFactory<TInterface>>(libraryHandle, factory, core::resources::Name{ name });
        }

        template<typename TInterface, typename TImplementation, typename... TArgs>
        void registerService(const generic::extension::LibraryHandle& libraryHandle, core::Registry& registry, TArgs&&... args)
        {
            auto instance = createInstance<TImplementation>(std::forward<TArgs>(args)...);
            registry.set<TInterface>(libraryHandle, instance);
        }

        template<typename TInterface, auto Identifier, typename TImplementation, typename... TArgs>
        void registerService(const generic::extension::LibraryHandle& libraryHandle, core::Registry& registry, TArgs&&... args)
        {
            auto instance = createInstance<TImplementation>(std::forward<TArgs>(args)...);
            registry.set<TInterface, Identifier>(libraryHandle, instance);
        }

        template<typename TInterface, typename TImplementation, typename TIdentifier, typename... TArgs>
        void registerService(TIdentifier identifier, const generic::extension::LibraryHandle& libraryHandle, core::Registry& registry, TArgs&&... args)
        {
            auto instance = createInstance<TImplementation>(std::forward<TArgs>(args)...);
            registry.set<TInterface>(libraryHandle, instance, identifier);
        }

        template<typename T, typename... TArgs>
        std::shared_ptr<T> createInstance(TArgs&&... args)
        {
            std::pmr::polymorphic_allocator<T> pa{ &_globalResource };
            return std::allocate_shared<T>(pa, std::forward<TArgs>(args)...);
        }

    public:
        explicit Setup(core::GlobalAllocatorType& globalAllocator):
            _globalAllocator(globalAllocator)
        {}

        void setupRegistry(const generic::extension::LibraryHandle& libraryHandle, core::Registry& registry,
                            const core::resources::configuration::Configuration& configuration) override;
    };
}