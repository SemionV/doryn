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

        auto& hippieColor = colorsUniformData.hippieColor;

        if(hippieColor.r >= 1.f || hippieColor.g  >= 1.f || hippieColor.b  >= 1.f)
        {
            hippieColorDelta = -colorDelta;
        }
        else if(hippieColor.r <= 0.f || hippieColor.g <= 0.f || hippieColor.b <= 0.f)
        {
            hippieColorDelta = colorDelta;
        }

        hippieColor.r += hippieColorDelta;
        hippieColor.g += hippieColorDelta;
        hippieColor.b += hippieColorDelta;

        services::OpenglService::setUniformBlockData(trianglesProgram.colorsUniformBlock, &colorsUniformData, sizeof(colorsUniformData));

        services::OpenglService::drawObject(trianglesProgram, trianglesVertexArray);
    }
}