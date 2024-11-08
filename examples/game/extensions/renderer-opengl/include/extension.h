#pragma once

#include "dory/generic/extension/iModule.h"
#include <dory/generic/extension/registryResourceScope.h>
#include <dory/core/registry.h>
#include "shaderProgramService.h"

namespace dory::renderer::opengl
{
    class Extension: public dory::generic::extension::IModule<dory::core::resources::DataContext>
    {
    private:
        core::Registry& _registry;
        core::RegistryResourceScopeWithId<core::services::IRenderer, core::resources::GraphicalSystem> _renderer;
        core::RegistryResourceScopeWithId<core::services::IGraphicalSystem, core::resources::GraphicalSystem> _graphicalSystem;
        ShaderRepository _shaderRepository;
        ProgramRepository _programRepository;
        ShaderProgramService _shaderProgramService;

    public:
        explicit Extension(core::Registry& registry);

        ~Extension() override;

        void attach(dory::generic::extension::LibraryHandle library, dory::core::resources::DataContext& dataContext) final;
    };
}