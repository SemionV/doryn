#include "dory/core/registry.h"
#include "dory/core/devices/openglGpuDevice.h"
#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include "spdlog/fmt/fmt.h"
#include "dory/core/resources/bindings/openglBufferBinding.h"
#include "dory/core/resources/bindings/openglMeshBinding.h"

namespace dory::core::devices
{
    using namespace resources;
    using namespace resources::bindings;
    using namespace resources::objects;
    using namespace dory::core::services;

    const char* getErrorString(GLenum errorCode)
    {
        switch (errorCode)
        {
            case GL_NO_ERROR:
                return "No error";
            case GL_INVALID_ENUM:
                return "Invalid enum";
            case GL_INVALID_VALUE:
                return "Invalid value";
            case GL_INVALID_OPERATION:
                return "Invalid operation";
            case GL_STACK_OVERFLOW:
                return "Stack overflow";
            case GL_STACK_UNDERFLOW:
                return "Stack underflow";
            case GL_OUT_OF_MEMORY:
                return "Out of memory";
            case GL_INVALID_FRAMEBUFFER_OPERATION:
                return "Invalid framebuffer operation";
                // Add more cases if needed
            default:
                return "Unknown error";
        }
    }

    GLenum getGlEnumType(const ComponentType componentType)
    {
        switch (componentType)
        {
            case ComponentType::floatType: return GL_FLOAT;
            case ComponentType::doubleType: return GL_DOUBLE;
            case ComponentType::uintType: return GL_UNSIGNED_INT;
        }
    }

    OpenglGpuDevice::OpenglGpuDevice(Registry& registry) : DependencyResolver(registry)
    {}

    void OpenglGpuDevice::connect(DataContext& context)
    {
        glfwInit();
        glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
        GLFWwindow* hidden_window = glfwCreateWindow(1, 1, "", NULL, NULL);
        glfwMakeContextCurrent(hidden_window);
        int version = gladLoadGL(glfwGetProcAddress);
        if (version == 0)
        {
            auto logger = _registry.get<core::services::ILogService>();
            if(logger)
            {
                logger->error(std::string_view("Failed to initialize OpenGL"));
            }
        }
        glfwDestroyWindow(hidden_window);
    }

    void OpenglGpuDevice::disconnect(DataContext& context)
    {}

    bool OpenglGpuDevice::checkForError()
    {
        GLenum errorCode = glGetError();
        if (errorCode != GL_NO_ERROR)
        {
            _registry.get<ILogService>([&errorCode](ILogService* logger) {
                logger->error(fmt::format("Error on allocating OpenGL buffer: {0}, {1}", errorCode, getErrorString(errorCode)));
            });

            return true;
        }

        return false;
    }

    bool OpenglGpuDevice::allocateBuffer(BufferBinding* bufferBinding, std::size_t size)
    {
        auto glBuffer = (OpenglBufferBinding*)bufferBinding;
        assert(glBuffer->glId == 0);
        assert(glIsBuffer(glBuffer->glId));

        glCreateBuffers(1, &glBuffer->glId);

        glNamedBufferStorage(glBuffer->glId, (GLsizeiptr)size, nullptr, GL_DYNAMIC_STORAGE_BIT);

        return !checkForError();
    }

    void OpenglGpuDevice::deallocateBuffer(BufferBinding* bufferBinding)
    {
        auto glBuffer = (OpenglBufferBinding*)bufferBinding;
        assert(glBuffer->glId != 0);
        assert(glIsBuffer(glBuffer->glId));

        glDeleteBuffers(1, &glBuffer->glId);
        glBuffer->glId = 0;
        glBuffer->size = 0;
    }

    void OpenglGpuDevice::writeData(BufferBinding* bufferBinding, std::size_t offset, std::size_t size, const void* data)
    {
        auto glBuffer = (OpenglBufferBinding*)bufferBinding;
        assert(glBuffer->glId != 0);

        glNamedBufferSubData(glBuffer->glId, (GLintptr)offset, (GLsizeiptr)size, data);
        checkForError();
    }

    void OpenglGpuDevice::setVertexAttributes(const MeshBinding* meshBinding, const BufferBinding* vertexBuffer, VertexAttributeBinding* attributes, const std::size_t count)
    {
        auto glMesh = (OpenglMeshBinding*)meshBinding;
        auto glVertexBuffer = (OpenglBufferBinding*)vertexBuffer;
        assert(glMesh->glVertexArrayId != 0);
        assert(glIsVertexArray(glMesh->glVertexArrayId));
        assert(glVertexBuffer->glId != 0);
        assert(glIsBuffer(glVertexBuffer->glId));

        glBindVertexArray(glMesh->glVertexArrayId);
        glBindBuffer(GL_ARRAY_BUFFER, glVertexBuffer->glId);
        for(std::size_t i = 0; i < count; ++i)
        {
            auto& attribute = attributes[i];

            glVertexAttribPointer(i, (GLint)attribute.componentsCount, getGlEnumType(attribute.componentType), attribute.normalized, (GLsizei)attribute.stride, (void*)attribute.offset);
            glEnableVertexAttribArray(i);
        }

        checkForError();
    }

    void OpenglGpuDevice::bindMesh(MeshBinding* meshBinding, const BufferBinding* vertexBuffer, const BufferBinding* indexBuffer)
    {
        auto glMesh = (OpenglMeshBinding*)meshBinding;

        if(glMesh->glVertexArrayId == 0 && !glIsVertexArray(glMesh->glVertexArrayId))
        {
            glCreateVertexArrays(1, &glMesh->glVertexArrayId);
            checkForError();

            if(vertexBuffer)
            {
                auto glVertexBuffer = (OpenglBufferBinding*)vertexBuffer;
                //TODO: bind VAB and EBO to VAO
                //glVertexArrayVertexBuffer(glMesh->glVertexArrayId, 0, glVertexBuffer->glId, glMesh->vertexBufferOffset, );
            }
        }
    }

    void drawMesh(const MeshBinding* meshBinding)
    {
        auto glMesh = (OpenglMeshBinding*)meshBinding;
        assert(glIsVertexArray(glMesh->glVertexArrayId));

        glBindVertexArray(glMesh->glVertexArrayId);
        if(glMesh->indexBufferId != nullId)
        {
            glDrawElements(GL_TRIANGLES, (GLsizei)glMesh->indexCount, getGlEnumType(glMesh->indexType), (void*)glMesh->indexBufferOffset);
        }
        else if(glMesh->glVertexArrayId != nullId)
        {
            glDrawArrays(GL_TRIANGLES, 0, (GLsizei)glMesh->vertexCount);
        }
    }

    void OpenglGpuDevice::drawFrame(const Frame& frame)
    {
        glClearColor(frame.clearColor.x, frame.clearColor.y, frame.clearColor.z, frame.clearColor.w);
        glClear(GL_COLOR_BUFFER_BIT);

        for(const auto* meshBinding : frame.meshes)
        {
            drawMesh(meshBinding);
        }

        glFlush();
    }
}