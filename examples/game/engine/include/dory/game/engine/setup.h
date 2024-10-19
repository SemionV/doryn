#pragma once

#include <dory/core/iSetup.h>
#include <dory/core/macros.h>

namespace dory::game::engine
{
    class DORY_DLLEXPORT Setup: public core::ISetup
    {
    private:
        template<typename TEventBundle>
        void registerEventBundle(const core::extensionPlatform::LibraryHandle& libraryHandle,core::Registry& registry)
        {
            auto instance = std::make_shared<core::events::DispatcherCannon<typename TEventBundle::IListener,
                    typename TEventBundle::IDispatcher,
                    typename TEventBundle::EventListType>>();

            registry.set<typename TEventBundle::IDispatcher, typename TEventBundle::IListener>(libraryHandle, instance);
        }

        template<typename TEventBundle>
        void registerEventBufferBundle(const core::extensionPlatform::LibraryHandle& libraryHandle,core::Registry& registry)
        {
            auto instance = std::make_shared<core::events::DispatcherCannonBuffer<typename TEventBundle::IListener,
                    typename TEventBundle::IDispatcher,
                    typename TEventBundle::EventListType>>();

            registry.set<typename TEventBundle::IDispatcher, typename TEventBundle::IListener>(libraryHandle, instance);
        }

    public:
        void setupRegistry(const core::extensionPlatform::LibraryHandle& libraryHandle, core::Registry& registry) override;
    };
}