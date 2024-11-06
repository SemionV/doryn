#pragma once

#include <dory/core/registry.h>
#include "shaderProgramService.h"

namespace dory::renderer::opengl
{
    class OpenglRenderer: public core::services::IRenderer
    {
    private:
        core::Registry& _registry;
        ShaderProgramService& _shaderProgramService;

    public:
        explicit OpenglRenderer(core::Registry& registry, ShaderProgramService& shaderProgramService);

        bool initialize(core::resources::DataContext& context) override;
        void draw(core::resources::DataContext& context, const core::resources::entities::Window& window, const core::resources::entities::View& view) override;
    };
}
