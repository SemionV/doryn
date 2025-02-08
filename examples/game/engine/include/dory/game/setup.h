#pragma once

#include "dory/core/iSetup.h"
#include "dory/generic/macros.h"
#include "dory/core/repository.h"
#include "dory/core/events.h"
#include "dory/core/services/objectFactory.h"

namespace dory::game
{
    class DORY_DLLEXPORT Setup: public core::ISetup
    {
    private:
        template<typename TEventBundle>
        void registerEventBundle(const generic::extension::LibraryHandle& libraryHandle, core::Registry& registry)
        {
            auto instance = std::make_shared<core::events::DispatcherCannon<typename TEventBundle::IListener,
                    typename TEventBundle::IDispatcher,
                    typename TEventBundle::EventListType>>();

            registry.set<typename TEventBundle::IDispatcher>(libraryHandle, instance);
            registry.set<typename TEventBundle::IListener>(libraryHandle, instance);
        }

        template<typename TEventBundle>
        void registerEventBufferBundle(const generic::extension::LibraryHandle& libraryHandle, core::Registry& registry)
        {
            auto instance = std::make_shared<core::events::DispatcherCannonBuffer<typename TEventBundle::IListener,
                    typename TEventBundle::IDispatcher,
                    typename TEventBundle::EventListType>>();

            registry.set<typename TEventBundle::IDispatcher>(libraryHandle, instance);
            registry.set<typename TEventBundle::IListener>(libraryHandle, instance);
        }

        template<typename TEntity>
        void registerRepository(const generic::extension::LibraryHandle& libraryHandle, core::Registry& registry)
        {
            auto instance = std::make_shared<core::repositories::Repository<TEntity>>();
            registry.set<core::repositories::IRepository<TEntity>>(libraryHandle, instance);
        }

        template<typename TInterface, typename TImplementation>
        void registerObjectFactory(const char* name, const generic::extension::LibraryHandle& libraryHandle, core::Registry& registry)
        {
            const auto factory = std::make_shared<core::services::ObjectFactory<TInterface, TImplementation>>(libraryHandle);
            registry.set<core::services::IObjectFactory<TInterface>>(libraryHandle, factory, core::resources::Name {name});
        }

    public:
        void setupRegistry(const generic::extension::LibraryHandle& libraryHandle, core::Registry& registry,
                            const core::resources::configuration::Configuration& configuration) override;
    };
}