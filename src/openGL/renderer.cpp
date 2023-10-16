#include "dependencies.h"
#include "renderer.h"

namespace dory::openGL
{
    void Renderer::initialize(std::shared_ptr<configuration::IConfiguration> configuration)
    {
        program = loadProgram(configuration);

        bindProgram(program);

        glGenVertexArrays( NumVAOs, VAOs );
        glBindVertexArray( VAOs[Triangles] );

        GLfloat  vertices[NumVertices][2] = {
            { -0.90f, -0.90f }, {  0.85f, -0.90f }, { -0.90f,  0.85f },  // Triangle 1
            {  0.90f, -0.85f }, {  0.90f,  0.90f }, { -0.85f,  0.90f }   // Triangle 2
        };

        glCreateBuffers( NumBuffers, Buffers );
        glBindBuffer( GL_ARRAY_BUFFER, Buffers[ArrayBuffer] );
        glBufferStorage( GL_ARRAY_BUFFER, sizeof(vertices), vertices, 0);

        glVertexAttribPointer(vPosition, 2, GL_FLOAT, GL_FALSE, 0, (void*)(0));
        glEnableVertexAttribArray(vPosition);

        /*auto triangleColorIndex = glGetUniformLocation(programId, "triangleColor");
        if(triangleColorIndex != GL_INVALID_INDEX)
        {
            glUniform4f(triangleColorIndex, 0.7, 0.7, 0.7, 1.0);
        }*/

        auto colorsBlockIndex = glGetUniformBlockIndex(program.id, "ColorsBlock");
        if(colorsBlockIndex != GL_INVALID_INDEX)
        {
            glBindBuffer( GL_UNIFORM_BUFFER, Buffers[UniformBuffer] );
            GLint blockSize {0};
            glGetActiveUniformBlockiv(program.id, colorsBlockIndex, GL_UNIFORM_BLOCK_DATA_SIZE, &blockSize);

            graphics::Buffer buffer;
            buffer.data = &colorsUniform;
            buffer.size = sizeof(colorsUniform);
            buffer.index = Buffers[UniformBuffer];

            colorsUniform.brightColor = domain::Color(0.7f, 0.7f, 0.7f, 1.0f);
            colorsUniform.hippieColor = domain::Color(0.7f, 0.7f, 0.0f, 1.0f);
            colorsUniform.darkColor = domain::Color(0.2f, 0.2f, 0.2f, 1.0f);

            glBufferStorage( GL_UNIFORM_BUFFER, buffer.size, buffer.data, 0);

            glBindBufferBase(GL_UNIFORM_BUFFER, colorsBlockIndex, buffer.index);

        }
    }

    void Renderer::draw()
    {
        static const float black[] = { 0.0f, 0.0f, 0.0f, 0.0f };

        glClearBufferfv(GL_COLOR, 0, black);

        glBindVertexArray( VAOs[Triangles] );
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

    void Renderer::bindProgram(graphics::Program program)
    {
        glUseProgram(program.id);
    }
}