#include "dependencies.h"
#include "viewControllerOpenGL.h"

namespace dory::openGL
{
    ViewControllerOpenGL::ViewControllerOpenGL(int viewId, 
            std::shared_ptr<RepositoryReader<domain::entity::View>> viewRepository, 
            std::shared_ptr<IConfiguration> configuration,
            std::shared_ptr<RepositoryReader<GlfwWindow>> windowRespository):
        ViewController(viewId, viewRepository, configuration),
        windowRespository(windowRespository)
    {
    }

    bool ViewControllerOpenGL::initialize(dory::DataContext& context)
    {
        std::cout << "initialize: OpenGL Basic View" << std::endl;

        auto view = viewRepository->get(viewId);
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
                verticiesShader.shaderSource = configuration->getTextFileContent("shaders/triangles/triangles.vert");
                verticiesShader.type = GL_VERTEX_SHADER;

                ShaderMetadata fragmentShader;
                fragmentShader.identifier = "shaders/triangles/triangles.frag";
                fragmentShader.shaderSource = configuration->getTextFileContent("shaders/triangles/triangles.frag");
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

                glVertexAttribPointer( vPosition, 2, GL_FLOAT, GL_FALSE, 0, (void*)(0) );
                glEnableVertexAttribArray( vPosition );
            }
        }

        return true;
    }

    void ViewControllerOpenGL::stop(dory::DataContext& context)
    {
    }

    void ViewControllerOpenGL::update(const int referenceId, const dory::TimeSpan& timeStep, dory::DataContext& context)
    {
        auto view = viewRepository->get(viewId);
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
}