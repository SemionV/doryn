#include "dependencies.h"
#include "renderer.h"
#include "services/openglService.h"
#include "services/shaderService.h"

namespace dory::openGL
{
    void Renderer::initialize(std::shared_ptr<configuration::IConfiguration> configuration)
    {
        services::ShaderService::loadProgram(trianglesProgram, configuration, [](services::ShaderServiceError& error)
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

        services::OpenglService::loadUniformBlock(trianglesProgram.id, trianglesProgram.colorsUniformBlock);
        services::OpenglService::setUniformBlockData(trianglesProgram.colorsUniformBlock, &colorsUniformData, sizeof(colorsUniformData));

        services::OpenglService::loadVertexArray(trianglesVertexArray);
        services::OpenglService::setVertexArrayData(trianglesVertexArray, verticesData.data(), sizeof(verticesData));
    }

    void Renderer::draw()
    {
        services::OpenglService::clearViewport(clearScreenColor);

        colorsUniformData.hippieColor.r = 1.f;
        colorsUniformData.hippieColor.g = 0.f;
        colorsUniformData.hippieColor.b = 0.f;
        services::OpenglService::setUniformBlockData(trianglesProgram.colorsUniformBlock, &colorsUniformData, sizeof(colorsUniformData));

        services::OpenglService::drawObject(trianglesProgram, trianglesVertexArray);
    }
}