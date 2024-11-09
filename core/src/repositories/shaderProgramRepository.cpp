#include <glad/gl.h>
#include <dory/core/repositories/shaderProgramRepository.h>

namespace dory::core::repositories
{
    resources::entities::ShaderProgram* ShaderProgramRepository::get(std::string_view key, resources::IdType windowId)
    {
        auto programs = this->getAll();
        for(auto& program: programs)
        {
            if(program.key == key && program.windowId == windowId)
            {
                return &program;
            }
        }

        return nullptr;
    }
}