#include <dory/core/registry.h>
#include <dory/core/services/graphics/openglGpuDriver.h>
#include <glad/gl.h>
#include <spdlog/fmt/fmt.h>
#include <dory/core/resources/bindings/openglBufferBinding.h>
#include <dory/core/resources/bindings/openglMeshBinding.h>

namespace dory::core::services::graphics
{
    using namespace resources;
    using namespace resources::bindings;
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

    GLenum getGlEnumType(ComponentType componentType)
    {
        switch (componentType)
        {
            case ComponentType::floatType: return GL_FLOAT;
            case ComponentType::doubleType: return GL_DOUBLE;
        }
    }

    OpenglGpuDriver::OpenglGpuDriver(Registry& registry) : DependencyResolver(registry)
    {}

    bool OpenglGpuDriver::checkForError()
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

    bool OpenglGpuDriver::allocateBuffer(BufferBinding* bufferBinding, std::size_t size)
    {
        auto glBuffer = (OpenglBufferBinding*)bufferBinding;
        assert(glBuffer->glId == 0);
        assert(glIsBuffer(glBuffer->glId));

        glCreateBuffers(1, &glBuffer->glId);

        glNamedBufferStorage(glBuffer->glId, (GLsizeiptr)size, nullptr, GL_DYNAMIC_STORAGE_BIT);

        return !checkForError();
    }

    void OpenglGpuDriver::deallocateBuffer(resources::bindings::BufferBinding* bufferBinding)
    {
        auto glBuffer = (OpenglBufferBinding*)bufferBinding;
        assert(glBuffer->glId != 0);
        assert(glIsBuffer(glBuffer->glId));

        glDeleteBuffers(1, &glBuffer->glId);
        glBuffer->glId = 0;
        glBuffer->size = 0;
    }

    void OpenglGpuDriver::writeData(resources::bindings::BufferBinding* bufferBinding, std::size_t offset, std::size_t size, const void* data)
    {
        auto glBuffer = (OpenglBufferBinding*)bufferBinding;
        assert(glBuffer->glId != 0);

        glNamedBufferSubData(glBuffer->glId, (GLintptr)offset, (GLsizeiptr)size, data);
        checkForError();
    }

    void OpenglGpuDriver::setVertexAttributes(const MeshBinding* meshBinding, const BufferBinding* vertexBufferBinding, VertexAttributeBinding* attributes, const std::size_t count)
    {
        auto glMesh = (OpenglMeshBinding*)meshBinding;
        auto glVertexBuffer = (OpenglBufferBinding*)vertexBufferBinding;
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

    void OpenglGpuDriver::initializeMesh(MeshBinding* meshBinding)
    {
        auto glMesh = (OpenglMeshBinding*)meshBinding;

        if(glMesh->glVertexArrayId == 0 && !glIsVertexArray(glMesh->glVertexArrayId))
        {
            glCreateVertexArrays(1, &glMesh->glVertexArrayId);
            checkForError();
        }
    }
}