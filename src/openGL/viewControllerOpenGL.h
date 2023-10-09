#pragma once

#include "glfwWindow.h"
#include "shaderService.h"

namespace dory::openGL
{
    template<class TDataContext>
    class ViewControllerOpenGL: public domain::ViewController<TDataContext, GlfwWindow>
    {
        private:
            std::shared_ptr<domain::RepositoryReader<GlfwWindow>> windowRespository;

            enum VAO_IDs { Triangles, NumVAOs };
            enum Buffer_IDs { ArrayBuffer, UniformBuffer, NumBuffers };
            enum Attrib_IDs { vPosition = 0 };

            GLuint  VAOs[NumVAOs];
            GLuint  Buffers[NumBuffers];
            enum {NumVertices = 6};

        public:
            ViewControllerOpenGL(std::shared_ptr<domain::RepositoryReader<domain::entity::View>> viewRepository, 
                    std::shared_ptr<configuration::IConfiguration> configuration,
                    std::shared_ptr<domain::RepositoryReader<GlfwWindow>> windowRespository):
                domain::ViewController<TDataContext, GlfwWindow>(viewRepository, configuration),
                windowRespository(windowRespository)
            {
            }

            bool initialize(domain::entity::IdType referenceId, TDataContext& context) override
            {
                std::cout << "initialize: OpenGL Basic View" << std::endl;

                auto view = this->viewRepository->get(referenceId, [](domain::entity::View* view, domain::entity::IdType referenceId)
                {
                    return view->controllerNodeId == referenceId;
                });

                if(view)
                {
                    auto glfwWindow = windowRespository->get(view->windowId);
                    if(glfwWindow)
                    {
                        auto windowHandler = glfwWindow->handler;
                        glfwMakeContextCurrent(windowHandler);

                        gl3wInit();

                        glGenVertexArrays( NumVAOs, VAOs );
                        glBindVertexArray( VAOs[Triangles] );

                        GLfloat  vertices[NumVertices][2] = {
                            { -0.90f, -0.90f }, {  0.85f, -0.90f }, { -0.90f,  0.85f },  // Triangle 1
                            {  0.90f, -0.85f }, {  0.90f,  0.90f }, { -0.85f,  0.90f }   // Triangle 2
                        };

                        glCreateBuffers( NumBuffers, Buffers );
                        glBindBuffer( GL_ARRAY_BUFFER, Buffers[ArrayBuffer] );
                        glBufferStorage( GL_ARRAY_BUFFER, sizeof(vertices), vertices, 0);

                        ShaderMetadata verticiesShader;
                        verticiesShader.identifier = "shaders/triangles/triangles.vert";
                        verticiesShader.shaderSource = this->configuration->getTextFileContent("shaders/triangles/triangles.vert");
                        verticiesShader.type = GL_VERTEX_SHADER;

                        ShaderMetadata fragmentShader;
                        fragmentShader.identifier = "shaders/triangles/triangles.frag";
                        fragmentShader.shaderSource = this->configuration->getTextFileContent("shaders/triangles/triangles.frag");
                        fragmentShader.type = GL_FRAGMENT_SHADER;

                        std::vector<ShaderMetadata> shadersMetadata = {verticiesShader, fragmentShader};

                        GLuint programId = ShaderService::buildProgram(shadersMetadata, [](ShaderServiceError& error)
                            {
                                if(error.shaderCompilationError)
                                {
                                    std::cerr << "Shader compilation error(" << error.shaderCompilationError->shaderIdentifier << "): " << error.shaderCompilationError->compilationLog << std::endl;
                                }
                                else if(error.shaderProgramLinkingError)
                                {
                                    std::cerr << "Shader program linking error: " << error.shaderProgramLinkingError->linkingLog << std::endl;
                                }

                                return false;
                            });
                        glUseProgram(programId);

                        glVertexAttribPointer(vPosition, 2, GL_FLOAT, GL_FALSE, 0, (void*)(0));
                        glEnableVertexAttribArray(vPosition);

                        auto triangleColorIndex = glGetUniformLocation(programId, "triangleColor");
                        if(triangleColorIndex != GL_INVALID_INDEX)
                        {
                            glUniform4f(triangleColorIndex, 0.7, 0.7, 0.7, 1.0);
                        }

                        static constexpr std::string_view blockNameLiteral = "ColorsBlock";
                        static constexpr std::string_view pointLiteral = ".";
                        static constexpr std::string_view prefixLiteral = dory::domain::JoinStringLiterals<blockNameLiteral, pointLiteral>;

                        static constexpr std::string_view brightColorLiteral = "brightColor";
                        static constexpr std::string_view hippieColorLiteral = "hippieColor";
                        static constexpr std::string_view darkColorLiteral = "darkColor";

                        static constexpr auto brightColorUniformName = dory::domain::JoinStringLiterals<prefixLiteral, brightColorLiteral>;
                        static constexpr auto hippieColorUniformName = dory::domain::JoinStringLiterals<prefixLiteral, hippieColorLiteral>;
                        static constexpr auto darkColorUniformName = dory::domain::JoinStringLiterals<prefixLiteral, darkColorLiteral>;

                        auto uniformBlock = makeUniformBlock(blockNameLiteral, {brightColorUniformName, hippieColorUniformName, darkColorUniformName});

                        bindUniformBlock(programId, uniformBlock);

                        auto colorsBlockIndex = glGetUniformBlockIndex(programId, "ColorsBlock");
                        if(colorsBlockIndex != GL_INVALID_INDEX)
                        {
                            glBindBuffer( GL_UNIFORM_BUFFER, Buffers[UniformBuffer] );
                            GLint blockSize {0};
                            glGetActiveUniformBlockiv(programId, colorsBlockIndex, GL_UNIFORM_BLOCK_DATA_SIZE, &blockSize);

                            GLfloat  colors[3][4] = {
                            { 0.7f, 0.7f, 0.7f, 1.0f }, 
                            { 0.7f, 0.7f, 0.0f, 1.0f },
                            { 0.2f, 0.2f, 0.2f, 1.0f }};

                            glBufferStorage( GL_UNIFORM_BUFFER, sizeof(colors), colors, 0);

                            glBindBufferBase(GL_UNIFORM_BUFFER, colorsBlockIndex, Buffers[UniformBuffer]);

                        }
                    }
                }

                return true;
            }

            struct Buffer
            {
                GLuint index = GL_INVALID_INDEX;
                GLuint size = 0;
                GLvoid* data = nullptr;
            };

            struct Uniform
            {
                std::string key;
                GLuint index = GL_INVALID_INDEX;
                GLint size = 0;
                GLint offset = 0;
                GLint type = 0;
            };

            template<std::size_t membersCount>
            struct UniformBlock
            {
                std::string key;
                GLuint index = GL_INVALID_INDEX;
                GLint size = 0;
                GLint bufferOffset = 0;
                std::array<Uniform, membersCount> members;

                UniformBlock(const char* blockName):
                    key(blockName)
                {}
            };

            template<typename T, std::size_t N>
            static constexpr decltype(auto) makeUniformBlock(const std::string_view& blockName, 
                const T(&memberNames)[N]) noexcept
            {
                UniformBlock<N> uniformBlock(blockName.data());

                for(std::size_t i = 0; i < N; ++i)
                {
                    uniformBlock.members[i].key = memberNames[i];
                }

                return uniformBlock;
            }

            template<std::size_t N>
            static constexpr void getUniformBlockMemberNames(const UniformBlock<N>& block, const char*(&memberNames)[N]) noexcept
            {
                for(std::size_t i = 0; i < N; ++i)
                {
                    memberNames[i] = block.members[i].key.c_str();
                }
            }

            template<std::size_t N>
            static void bindUniformBlock(GLuint programId, UniformBlock<N>& block)
            {
                block.index = glGetUniformBlockIndex(programId, block.key.c_str());
                if(block.index != GL_INVALID_INDEX)
                {
                    glGetActiveUniformBlockiv(programId, block.index, GL_UNIFORM_BLOCK_DATA_SIZE, &block.size);
                    
                    GLuint memberIndices[N];
                    GLint memberOffset[N];
                    GLint memberSize[N];
                    GLint memberType[N];

                    const char* memberNames[N];
                    getUniformBlockMemberNames(block, memberNames);

                    glGetUniformIndices(programId, N, memberNames, memberIndices);
                    glGetActiveUniformsiv(programId, N, memberIndices, GL_UNIFORM_OFFSET, memberOffset);
                    glGetActiveUniformsiv(programId, N, memberIndices, GL_UNIFORM_SIZE, memberSize);
                    glGetActiveUniformsiv(programId, N, memberIndices, GL_UNIFORM_TYPE, memberType);

                    for(std::size_t i = 0; i < N; ++i)
                    {
                        Uniform& member = block.members[i];
                        member.index = memberIndices[i];
                        member.offset = memberOffset[i];
                        member.type = memberType[i];
                        member.size = memberSize[i];
                    }
                }
            }

            Buffer allocateBuffer(GLint bufferSize)
            {
                Buffer buffer;
                buffer.size = bufferSize;
                buffer.data = malloc(bufferSize);
                if(buffer.data == nullptr)
                {
                    buffer.size = 0;
                }

                return buffer;
            }

            void stop(domain::entity::IdType referenceId, TDataContext& context) override
            {

            }

            void update(domain::entity::IdType referenceId, const domain::TimeSpan& timeStep, TDataContext& context) override
            {
                auto view = this->viewRepository->get(referenceId, [](domain::entity::View* view, domain::entity::IdType referenceId)
                {
                    return view->controllerNodeId == referenceId;
                });

                if(view)
                {
                    auto glfwWindow = windowRespository->get(view->windowId);
                    if(glfwWindow)
                    {
                        auto windowHandler = glfwWindow->handler;

                        glfwMakeContextCurrent(windowHandler);

                        static const float black[] = { 0.0f, 0.0f, 0.0f, 0.0f };

                        glClearBufferfv(GL_COLOR, 0, black);

                        glBindVertexArray( VAOs[Triangles] );
                        glDrawArrays( GL_TRIANGLES, 0, NumVertices );

                        glfwSwapBuffers(windowHandler);
                    }
                }
            }
    };
}