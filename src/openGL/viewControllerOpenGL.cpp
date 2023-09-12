#include "dependencies.h"
#include "viewControllerOpenGL.h"

namespace doryOpenGL
{
    ViewControllerOpenGL::ViewControllerOpenGL(std::shared_ptr<dory::IConfiguration> configuration, std::shared_ptr<dory::View> view):
        ViewController(configuration, view)
    {
    }

    void ViewControllerOpenGL::initialize(dory::DataContext& context)
    {
        ViewController::initialize(context);

        std::cout << "initialize: OpenGL Basic View" << std::endl;

        auto glfwWindow = std::static_pointer_cast<GlfwWindow>(view->window);
        glfwMakeContextCurrent(glfwWindow->handler);

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

        GLuint programId = ShaderService::buildProgram(shadersMetadata);
        glUseProgram(programId);

        glVertexAttribPointer( vPosition, 2, GL_FLOAT, GL_FALSE, 0, (void*)(0) );
        glEnableVertexAttribArray( vPosition );
    }

    void ViewControllerOpenGL::stop(dory::DataContext& context)
    {
        auto glfwWindow = std::static_pointer_cast<GlfwWindow>(view->window);
        glfwDestroyWindow(glfwWindow->handler);
    }

    void ViewControllerOpenGL::update(const dory::TimeSpan& timeStep, dory::DataContext& context)
    {
        auto glfwWindow = std::static_pointer_cast<GlfwWindow>(view->window);
        glfwMakeContextCurrent(glfwWindow->handler);

        if(!glfwWindowShouldClose(glfwWindow->handler))
        {
            static const float black[] = { 0.0f, 0.0f, 0.0f, 0.0f };

            glClearBufferfv(GL_COLOR, 0, black);

            glBindVertexArray( VAOs[Triangles] );
            glDrawArrays( GL_TRIANGLES, 0, NumVertices );

            glfwSwapBuffers(glfwWindow->handler);
        }
        else
        {
            context.isStop = true;
        }
    }
}