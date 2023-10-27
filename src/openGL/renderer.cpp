#include "dependencies.h"
#include "renderer.h"
#include "graphics/openglProcedures.h"
#include "shaderService.h"

namespace dory::openGL
{
    void Renderer::initialize(std::shared_ptr<configuration::IConfiguration> configuration)
    {
        ShaderService::loadProgram(trianglesProgram, configuration, [](ShaderServiceError& error)
            {
                if(error.shaderCompilationError)
                {
                    std::cerr << "Shader compilation error(" << error.shaderCompilationError->shaderIdentifier << "): " 
                        << error.shaderCompilationError->compilationLog << std::endl;
                }
                else if(error.shaderProgramLinkingError)
                {
                    std::cerr << "Shader program linking error: " << error.shaderProgramLinkingError->linkingLog << std::endl;
                }
            });

        graphics::OpenglProcedures::loadUniformBlock(trianglesProgram.id, trianglesProgram.colorsUniformBlock);
        graphics::OpenglProcedures::setUniformBlockData(trianglesProgram.colorsUniformBlock, &colorsUniformData, sizeof(colorsUniformData));

        graphics::OpenglProcedures::loadVertexArray(trianglesVertexArray);
        graphics::OpenglProcedures::setVertexArrayData(trianglesVertexArray, verticesData.data(), sizeof(verticesData));
    }

    void Renderer::draw()
    {
        static const float black[] = { 0.0f, 0.0f, 0.0f, 0.0f };

        glClearBufferfv(GL_COLOR, 0, black);

        glBindVertexArray( trianglesVertexArray.id );
        graphics::OpenglProcedures::useProgram(trianglesProgram);

        glDrawArrays(GL_TRIANGLES, 0, trianglesVertexArray.verticesCount);
    }
}