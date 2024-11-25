#include <glad/gl.h>
#include <dory/core/repositories/shaderProgramRepository.h>

namespace dory::core::repositories
{
    resources::entities::ShaderProgram* ShaderProgramRepository::get(std::string_view key, resources::IdType windowId)
    {
        resources::entities::ShaderProgram* result = scan([&key, windowId](auto& program) {
            return program.key == key && program.windowId == windowId;
        });

        return result;
    }
}