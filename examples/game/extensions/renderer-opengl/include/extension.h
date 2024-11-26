#pragma once

#include "dory/generic/extension/iModule.h"
#include <dory/generic/extension/registryResourceScope.h>
#include <dory/core/registry.h>

namespace dory::renderer::opengl
{
    class Extension: public dory::generic::extension::IModule<dory::core::resources::DataContext>
    {
    private:
        core::Registry& _registry;
        core::RegistryResourceScopeWithId<core::services::graphics::IShaderService, core::resources::GraphicalSystem> _shaderService;

    public:
        explicit Extension(core::Registry& registry);

        ~Extension() override;

        void attach(dory::generic::extension::LibraryHandle library, dory::core::resources::DataContext& dataContext) final;
    };
}