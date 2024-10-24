#pragma once

#include <dory/core/iSetup.h>
#include <dory/core/macros.h>
#include <dory/core/generic/repository.h>

namespace dory::game::engine
{
    class DORY_DLLEXPORT Setup: public core::ISetup
    {
    private:
        template<typename TEventBundle>
        void registerEventBundle(const generic::extension::LibraryHandle& libraryHandle, core::Registry& registry)
        {
            auto instance = std::make_shared<core::generic::events::DispatcherCannon<typename TEventBundle::IListener,
                    typename TEventBundle::IDispatcher,
                    typename TEventBundle::EventListType>>();

            registry.set<typename TEventBundle::IDispatcher>(libraryHandle, instance);
            registry.set<typename TEventBundle::IListener>(libraryHandle, instance);
        }

        template<typename TEventBundle>
        void registerEventBufferBundle(const generic::extension::LibraryHandle& libraryHandle, core::Registry& registry)
        {
            auto instance = std::make_shared<core::generic::events::DispatcherCannonBuffer<typename TEventBundle::IListener,
                    typename TEventBundle::IDispatcher,
                    typename TEventBundle::EventListType>>();

            registry.set<typename TEventBundle::IDispatcher>(libraryHandle, instance);
            registry.set<typename TEventBundle::IListener>(libraryHandle, instance);
        }

        template<typename TEntity>
        void registerRepository(const generic::extension::LibraryHandle& libraryHandle, core::Registry& registry)
        {
            auto instance = std::make_shared<core::generic::repository::Repository<TEntity>>();
            registry.set<core::repositories::IRepository<TEntity>>(libraryHandle, instance);
        }

    public:
        void setupRegistry(const generic::extension::LibraryHandle& libraryHandle, core::Registry& registry) override;
    };
}