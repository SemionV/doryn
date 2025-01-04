#pragma once

#include <dory/core/dependencyResolver.h>
#include "dory/core/resources/dataContext.h"
#include "dory/generic/extension/libraryHandle.h"
#include "cameraService.h"

namespace dory::game::logic
{
    class MainSceneKeyboardHandler: public core::DependencyResolver
    {
    private:
        CameraService& _cameraService;

        core::resources::entities::View* getWindowView(core::resources::IdType windowId, core::resources::WindowSystem windowSystem);

    public:
        explicit MainSceneKeyboardHandler(core::Registry& registry, CameraService& cameraService);

        bool initialize(const dory::generic::extension::LibraryHandle& libraryHandle, core::resources::DataContext& context);
    };
}