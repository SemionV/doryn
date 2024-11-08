#pragma once

#include <dory/core/registry.h>
#include "shaderProgramService.h"

namespace dory::renderer::opengl
{
    class OpenglRenderer: public core::services::graphics::IRenderer
    {
    private:
        core::Registry& _registry;
        ShaderProgramService& _shaderProgramService;

    public:
        explicit OpenglRenderer(core::Registry& registry, ShaderProgramService& shaderProgramService);

        void draw(core::resources::DataContext& context, const core::resources::entities::View& view) override;
    };
}
