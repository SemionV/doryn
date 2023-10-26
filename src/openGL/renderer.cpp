#include "dependencies.h"
#include "renderer.h"

namespace dory::openGL
{
    void Renderer::initialize(std::shared_ptr<configuration::IConfiguration> configuration)
    {
        program = loadProgram(configuration); //TODO: this needs rework. Split onto two parts: 1. Build Program object and load shaders, 2. Bind program to OpenGL and compile
        graphics::OpenglProcedures::loadUniformBlock(program.id, colorsUniformBlock);//TODO: move Unifrom Block to Program object
        graphics::OpenglProcedures::setUniformBlockData(colorsUniformBlock, &colorsUniformData, sizeof(colorsUniformData));

        graphics::OpenglProcedures::loadVertexArray(trianglesVertexArray);
        graphics::OpenglProcedures::setVertexArrayData(trianglesVertexArray, verticesData.data(), sizeof(verticesData));
    }

    void Renderer::draw()
    {
        static const float black[] = { 0.0f, 0.0f, 0.0f, 0.0f };

        glClearBufferfv(GL_COLOR, 0, black);

        glBindVertexArray( trianglesVertexArray.id );
        graphics::OpenglProcedures::useProgram(program);

        glDrawArrays(GL_TRIANGLES, 0, trianglesVertexArray.verticesCount);
    }

    graphics::Program Renderer::loadProgram(std::shared_ptr<configuration::IConfiguration> configuration)
    {
        ShaderMetadata verticesShader;
        verticesShader.identifier = "shaders/triangles/triangles.vert";
        verticesShader.shaderSource = configuration->getTextFileContent(verticesShader.identifier);
        verticesShader.type = GL_VERTEX_SHADER;

        ShaderMetadata fragmentShader;
        fragmentShader.identifier = "shaders/triangles/triangles.frag";
        fragmentShader.shaderSource = configuration->getTextFileContent(fragmentShader.identifier);
        fragmentShader.type = GL_FRAGMENT_SHADER;

        graphics::Program program({ verticesShader, fragmentShader });

        program.id = ShaderService::buildProgram(program.shaders, [](ShaderServiceError& error)
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

                return false;
            });

        return program;
    }
}