#include "dependencies.h"
#include "renderer.h"

namespace dory::openGL
{
    void Renderer::initialize(std::shared_ptr<configuration::IConfiguration> configuration)
    {
        program = loadProgram(configuration);
        graphics::OpenglProcedures::bindProgram(program);

        graphics::OpenglProcedures::loadVertexArray(trianglesVertexArray);
        graphics::OpenglProcedures::bindVertexArray(trianglesVertexArray);

        GLfloat  vertices[NumVertices][2] = {
            { -0.90f, -0.90f }, {  0.85f, -0.90f }, { -0.90f,  0.85f },  // Triangle 1
            {  0.90f, -0.85f }, {  0.90f,  0.90f }, { -0.85f,  0.90f }   // Triangle 2
        };

        glCreateBuffers( NumBuffers, Buffers );
        glBindBuffer( GL_ARRAY_BUFFER, Buffers[ArrayBuffer] );
        glBufferStorage( GL_ARRAY_BUFFER, sizeof(vertices), vertices, 0);

        for(std::size_t i = 0; i < trianglesVertexArray.vertexAttributes.size(); ++i)
        {
            graphics::VertexAttribute& attribute = trianglesVertexArray.vertexAttributes[i];
            glVertexAttribPointer(i, attribute.count, attribute.type, attribute.normalized, attribute.stride, attribute.pointer);
            glEnableVertexAttribArray(i);
        }

        graphics::OpenglProcedures::loadUniformBlock(program.id, colorsUniformBlock);

        if(colorsUniformBlock.index != graphics::unboundId)
        {
            glBindBuffer( GL_UNIFORM_BUFFER, Buffers[UniformBuffer] );

            auto buffer = colorsUniformBlock.buffer;
            buffer.data = &colorsUniformBlock.colors;
            buffer.size = sizeof(colorsUniformBlock.colors);
            buffer.index = Buffers[UniformBuffer];

            colorsUniformBlock.colors.brightColor = domain::Color(0.7f, 0.7f, 0.7f, 1.0f);
            colorsUniformBlock.colors.hippieColor = domain::Color(0.7f, 0.7f, 0.0f, 1.0f);
            colorsUniformBlock.colors.darkColor = domain::Color(0.2f, 0.2f, 0.2f, 1.0f);

            glBufferStorage( GL_UNIFORM_BUFFER, buffer.size, buffer.data, 0);
            
            glBindBufferBase(GL_UNIFORM_BUFFER, colorsUniformBlock.index, buffer.index);

        }
    }

    void Renderer::draw()
    {
        static const float black[] = { 0.0f, 0.0f, 0.0f, 0.0f };

        glClearBufferfv(GL_COLOR, 0, black);

        glBindVertexArray( trianglesVertexArray.id );
        glDrawArrays( GL_TRIANGLES, 0, NumVertices );
    }

    graphics::Program Renderer::loadProgram(std::shared_ptr<configuration::IConfiguration> configuration)
    {
        ShaderMetadata verticiesShader;
        verticiesShader.identifier = "shaders/triangles/triangles.vert";
        verticiesShader.shaderSource = configuration->getTextFileContent("shaders/triangles/triangles.vert");
        verticiesShader.type = GL_VERTEX_SHADER;

        ShaderMetadata fragmentShader;
        fragmentShader.identifier = "shaders/triangles/triangles.frag";
        fragmentShader.shaderSource = configuration->getTextFileContent("shaders/triangles/triangles.frag");
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