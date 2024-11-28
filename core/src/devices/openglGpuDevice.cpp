#include "dory/core/registry.h"
#include "dory/core/devices/openglGpuDevice.h"
#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include "spdlog/fmt/fmt.h"
#include "dory/core/resources/bindings/openglBufferBinding.h"
#include "dory/core/resources/bindings/openglMeshBinding.h"
#include "dory/core/resources/bindings/openglShaderBinding.h"
#include "dory/core/resources/bindings/openglMaterialBinding.h"
#include "dory/core/resources/bindings/uniforms.h"
#include "dory/core/resources/uniformsRefl.h"
#include "dory/core/services/graphics/uniformVisitor.h"

namespace dory::core::devices
{
    using namespace resources;
    using namespace resources::bindings;
    using namespace resources::bindings::uniforms;
    using namespace resources::objects;
    using namespace services;

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

    GLenum getGlType(const ComponentType componentType)
    {
        switch (componentType)
        {
            case ComponentType::floatType: return GL_FLOAT;
            case ComponentType::doubleType: return GL_DOUBLE;
            case ComponentType::uintType: return GL_UNSIGNED_INT;
        }

        assert(false);
    }

    std::size_t getComponentSite(const ComponentType componentType)
    {
        switch (componentType)
        {
            case ComponentType::floatType: return sizeof(float );
            case ComponentType::doubleType: return sizeof(double );
            case ComponentType::uintType: return sizeof(unsigned int);
        }

        assert(false);
    }

    GLenum getGlShaderType(const resources::assets::ShaderType type)
    {
        switch (type)
        {
            case resources::assets::ShaderType::vertex: return GL_VERTEX_SHADER;
            case resources::assets::ShaderType::fragment: return GL_FRAGMENT_SHADER;
        }

        assert(false);
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
            auto logger = _registry.get<ILogService>();
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
                logger->error(fmt::format("OpenGL error: {0}, {1}", errorCode, getErrorString(errorCode)));
            });

            return true;
        }

        return false;
    }

    bool OpenglGpuDevice::allocateBuffer(BufferBinding* bufferBinding, std::size_t size)
    {
        auto glBuffer = (OpenglBufferBinding*)bufferBinding;
        assert(glBuffer->glId == 0);

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
                glVertexArrayAttribFormat(glMesh->glVertexArrayId, i, (GLint)attribute.componentsCount,
                                          getGlType(attribute.componentType), attribute.normalized, (GLuint)attribute.offset);
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

    void OpenglGpuDevice::bindShader(const std::string& sourceCode, resources::assets::ShaderType type, ShaderBinding* shaderBinding)
    {
        auto glShader = (OpenglShaderBinding*)shaderBinding;

        glShader->type = getGlShaderType(type);
        glShader->glId = glCreateShader(glShader->type);

        const char* shaderSource = sourceCode.c_str();
        const auto length = (GLint)sourceCode.length();
        glShaderSource(glShader->glId, 1, &shaderSource, &length);
        glCompileShader(glShader->glId);

        GLint compiled;
        glGetShaderiv(glShader->glId, GL_COMPILE_STATUS, &compiled);
        if (!compiled)
        {
            GLsizei messageLength;
            glGetShaderiv(glShader->glId, GL_INFO_LOG_LENGTH, &messageLength);

            shaderBinding->compilationError = std::string(messageLength + 1, 0);
            glGetShaderInfoLog(glShader->glId, messageLength, &messageLength, shaderBinding->compilationError.data());
        }
    }

    class UniformLocationBinder
    {
    public:
        template<typename TUniform>
        static void process(const std::string_view& memberName, const std::size_t uniformId, const TUniform& value, OpenglMaterialBinding& material)
        {
            GLint location = glGetUniformLocation(material.glProgramId, memberName.data());
            if(location >= 0)
            {
                material.uniformLocations[uniformId] = location;
            }
        }

        static void process(const std::string_view& memberName, const std::size_t uniformId, const Material& value, OpenglMaterialBinding& material)
        {
            GLuint blockIndex = glGetUniformBlockIndex(material.glProgramId, memberName.data());
            if(blockIndex != 0)
            {
                //TODO: bind block
            }
        }
    };

    void bindUniformLocations(OpenglMaterialBinding* materialBinding)
    {
        services::graphics::UniformVisitor<UniformLocationBinder>::visit(Uniforms{}, *materialBinding);
    }

    void OpenglGpuDevice::bindMaterial(MaterialBinding* materialBinding, const std::vector<ShaderBinding*>& shaders)
    {
        auto glMaterial = (OpenglMaterialBinding*)materialBinding;

        glMaterial->glProgramId = glCreateProgram();
        assert(glIsProgram(glMaterial->glProgramId));

        for(const auto* shader : shaders)
        {
            const auto glShader = (OpenglShaderBinding*)shader;
            assert(glIsShader(glShader->glId));
            glAttachShader(glMaterial->glProgramId, glShader->glId);
        }

        glLinkProgram(glMaterial->glProgramId);

        GLint linked;
        glGetProgramiv(glMaterial->glProgramId, GL_LINK_STATUS, &linked);
        if (!linked)
        {
            GLsizei messageLength;
            glGetProgramiv(glMaterial->glProgramId, GL_INFO_LOG_LENGTH, &messageLength);

            glMaterial->linkingError = std::string(messageLength + 1, 0);
            glGetProgramInfoLog(glMaterial->glProgramId, messageLength, &messageLength, glMaterial->linkingError.data());
        }
        else
        {
            assert(glIsProgram(glMaterial->glProgramId));
            glUseProgram(glMaterial->glProgramId);
            bindUniformLocations(glMaterial);
        }
    }

    class UniformValueBinder
    {
    public:
        template<typename TUniform>
        static void process(const std::string_view& memberName, const std::size_t uniformId, const TUniform& value, OpenglMaterialBinding& material)
        {}

        static void process(const std::string_view& memberName, const std::size_t uniformId, const math::Vector4f value, OpenglMaterialBinding& material)
        {
            if(material.uniformLocations.contains(uniformId))
            {
                auto location = material.uniformLocations[uniformId];
                glUniform4f(location, value.x, value.y, value.z, value.w);
            }
        }

        static void process(const std::string_view& memberName, const std::size_t uniformId, const Material& value, OpenglMaterialBinding& material)
        {
        }
    };

    void bindUniformValues(const Uniforms& uniforms, OpenglMaterialBinding* materialBinding)
    {
        services::graphics::UniformVisitor<UniformValueBinder>::visit(uniforms, *materialBinding);
    }

    void fillUniforms(Uniforms& uniforms, const MaterialBinding* materialBinding)
    {
        if(materialBinding)
        {
            uniforms.color =  materialBinding->color;
        }
    }

    void setActiveMaterial(const Uniforms& uniforms, const MaterialBinding* materialBinding)
    {
        auto glMaterial = (OpenglMaterialBinding*)materialBinding;

        if(glMaterial && glMaterial->linkingError.empty())
        {
            assert(glIsProgram(glMaterial->glProgramId));
            glUseProgram(glMaterial->glProgramId);
            bindUniformValues(uniforms, glMaterial);
        }
    }

    void drawMesh(const MeshBinding* meshBinding)
    {
        auto glMesh = (OpenglMeshBinding*)meshBinding;
        assert(glIsVertexArray(glMesh->glVertexArrayId));

        glBindVertexArray(glMesh->glVertexArrayId);
        if(glMesh->indexBufferId != nullId)
        {
            glDrawElements(GL_TRIANGLES, (GLsizei)glMesh->indexCount, getGlType(glMesh->indexType), (void*)glMesh->indexBufferOffset);
        }
        else
        {
            glDrawArrays(GL_TRIANGLES, 0, (GLsizei)glMesh->vertexCount);
        }
    }

    void OpenglGpuDevice::drawFrame(const Frame& frame)
    {
        glClearColor(frame.clearColor.x, frame.clearColor.y, frame.clearColor.z, frame.clearColor.w);
        glClear(GL_COLOR_BUFFER_BIT);

        Uniforms uniforms;

        for(const auto& [material, meshes] : frame.meshMap)
        {
            fillUniforms(uniforms, material);
            setActiveMaterial(uniforms, material);
            for(const auto meshBinding : meshes)
            {
                drawMesh(meshBinding);
            }
        }

        glFlush();
    }
}