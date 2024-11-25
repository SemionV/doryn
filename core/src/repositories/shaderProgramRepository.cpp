#include <glad/gl.h>
#include <dory/core/repositories/shaderProgramRepository.h>

namespace dory::core::repositories
{
    resources::entities::ShaderProgram* ShaderProgramRepository::get(std::string_view key, resources::IdType windowId)
    {
        resources::entities::ShaderProgram* result = nullptr;
        scan([&key, windowId, &result](auto& program) {
            if(program.key == key && program.windowId == windowId)
            {
                result = &program;
                return true;
            }

            return false;
        });

        return result;
    }
}