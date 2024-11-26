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

    std::size_t getComponentSite(const ComponentType componentType)
    {
        switch (componentType)
        {
            case ComponentType::floatType: return sizeof(float );
            case ComponentType::doubleType: return sizeof(double );
            case ComponentType::uintType: return sizeof(unsigned int);
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

    void OpenglGpuDevice::bindMesh(MeshBinding* meshBinding, const BufferBinding* vertexBuffer, const BufferBinding* indexBuffer)
    {
        auto glMesh = (OpenglMeshBinding*)meshBinding;

        if(glMesh->glVertexArrayId == 0 && !glIsVertexArray(glMesh->glVertexArrayId))
        {
            glCreateVertexArrays(1, &glMesh->glVertexArrayId);
            checkForError();
        }

        assert(glIsVertexArray(glMesh->glVertexArrayId));

        if(vertexBuffer)
        {
            auto glVertexBuffer = (OpenglBufferBinding*)vertexBuffer;

            std::size_t i = 0;
            std::size_t offset = glMesh->vertexBufferOffset;
            for(const auto& attribute : glMesh->vertexAttributes)
            {
                std::size_t stride = attribute.componentsCount * getComponentSite(attribute.componentType);
                glVertexArrayVertexBuffer(glMesh->glVertexArrayId, i, glVertexBuffer->glId, (GLintptr)offset, (GLsizei)stride);
                offset += stride * glMesh->vertexCount;
                glVertexArrayAttribFormat(glMesh->glVertexArrayId, i, (GLint)attribute.componentsCount, getGlEnumType(attribute.componentType), attribute.normalized, (GLuint)attribute.offset);
                glEnableVertexArrayAttrib(glMesh->glVertexArrayId, i);
                ++i;
            }
        }

        if(indexBuffer)
        {
            auto glElementBuffer = (OpenglBufferBinding*)indexBuffer;
            glVertexArrayElementBuffer(glMesh->glVertexArrayId, glElementBuffer->glId);
        }

        checkForError();
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