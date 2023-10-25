#include "dependencies.h"
#include "renderer.h"

namespace dory::openGL
{
    void Renderer::initialize(std::shared_ptr<configuration::IConfiguration> configuration)
    {
        program = loadProgram(configuration); //TODO: this needs rework. Split onto two parts: 1. Build Program object and load shaders, 2. Bind program to OpenGL and compile

        graphics::OpenglProcedures::loadVertexArray(trianglesVertexArray);//TODO: Move Program to VertexArray
        graphics::OpenglProcedures::setVertexArrayData(trianglesVertexArray, 
            trianglesVertexArray.verticesData.data(), 
            sizeof(trianglesVertexArray.verticesData));

        graphics::OpenglProcedures::loadUniformBlock(program.id, colorsUniformBlock);//TODO: move Unifrom Block to Program object
        if(colorsUniformBlock.index != graphics::unboundId)
        {
            glCreateBuffers(1, &colorsUniformBlock.buffer.index);
            glBindBuffer(GL_UNIFORM_BUFFER, colorsUniformBlock.buffer.index);

            auto buffer = colorsUniformBlock.buffer;
            buffer.data = &colorsUniformBlock.colors;
            buffer.size = sizeof(colorsUniformBlock.colors);

            colorsUniformBlock.colors.brightColor = domain::Color(0.7f, 0.7f, 0.7f, 1.0f);
            colorsUniformBlock.colors.hippieColor = domain::Color(0.7f, 0.7f, 0.0f, 1.0f);
            colorsUniformBlock.colors.darkColor = domain::Color(0.2f, 0.2f, 0.2f, 1.0f);

            glBufferStorage( GL_UNIFORM_BUFFER, buffer.size, buffer.data, 0);

            glBindBufferBase(GL_UNIFORM_BUFFER, colorsUniformBlock.index, colorsUniformBlock.buffer.index);

        }
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
        ShaderMetadata verticiesShader;
        verticiesShader.identifier = "shaders/triangles/triangles.vert";
        verticiesShader.shaderSource = configuration->getTextFileContent(verticiesShader.identifier);
        verticiesShader.type = GL_VERTEX_SHADER;

        ShaderMetadata fragmentShader;
        fragmentShader.identifier = "shaders/triangles/triangles.frag";
        fragmentShader.shaderSource = configuration->getTextFileContent(fragmentShader.identifier);
        fragmentShader.type = GL_FRAGMENT_SHADER;

        graphics::Program program({ verticiesShader, fragmentShader });

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