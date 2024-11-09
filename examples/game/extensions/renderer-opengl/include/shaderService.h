#pragma once

#include <dory/core/services/graphics/iShaderService.h>

namespace dory::core
{
    class Registry;
}

namespace dory::renderer::opengl
{
    class ShaderService: public core::services::graphics::IShaderService
    {
    private:
        core::Registry& _registry;

    public:
        explicit ShaderService(core::Registry& registry);

        bool initializeProgram(core::resources::entities::ShaderProgram& program) override;
        bool initializeShader(const core::resources::entities::ShaderProgram& program,
                              core::resources::entities::Shader& shader, const std::string& sourceCode) override;
    };
}
