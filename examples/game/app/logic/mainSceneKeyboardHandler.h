#pragma once

#include <dory/core/dependencyResolver.h>
#include "dory/core/resources/dataContext.h"
#include "dory/generic/extension/libraryHandle.h"

namespace dory::game::logic
{
    class MainSceneKeyboardHandler: public core::DependencyResolver
    {
    public:
        explicit MainSceneKeyboardHandler(core::Registry& registry);

        bool initialize(const dory::generic::extension::LibraryHandle& libraryHandle, core::resources::DataContext& context);
    };
}