#pragma once

#include <dory/core/registry.h>

namespace dory::game
{
    class Game
    {
    private:
        core::Registry& _registry;

    public:
        explicit Game(core::Registry& registry):
            _registry(registry)
        {}

        bool initialize(core::resources::DataContext& context)
        {
            _registry.get<dory::core::services::IWindowService>([&context](dory::core::services::IWindowService* windowService) {
                context.mainWindowId = windowService->createWindow({800, 600, "dory game"});
            });

            _registry.get<dory::core::repositories::IViewRepository>([&context](dory::core::repositories::IViewRepository* viewRepository) {
                auto view = dory::core::resources::entity::View{dory::core::resources::nullId, context.mainWindowId};
                viewRepository->insert(view);
            });

            return true;
        }
    };
}
