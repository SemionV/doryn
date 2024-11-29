#include "dory/core/registry.h"
#include "dory/core/devices/openglGpuDevice.h"
#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include "spdlog/fmt/fmt.h"
#include "dory/serialization/reflection.h"
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

    static std::size_t getGlTypeSize(GLenum type)
    {
        std::size_t size = 0;

#define CASE(Enum, Count, Type) \
                case Enum: size = (Count) * sizeof(Type); break

        switch (type)
        {
            CASE(GL_FLOAT, 1, GLfloat);
            CASE(GL_FLOAT_VEC2, 2, GLfloat);
            CASE(GL_FLOAT_VEC3, 3, GLfloat);
            CASE(GL_FLOAT_VEC4, 4, GLfloat);
            CASE(GL_INT, 1, GLint);
            CASE(GL_INT_VEC2, 2, GLint);
            CASE(GL_INT_VEC3, 3, GLint);
            CASE(GL_INT_VEC4, 4, GLint);
            CASE(GL_UNSIGNED_INT, 1, GLuint);
            CASE(GL_UNSIGNED_INT_VEC2, 2, GLuint);
            CASE(GL_UNSIGNED_INT_VEC3, 3, GLuint);
            CASE(GL_UNSIGNED_INT_VEC4, 4, GLuint);
            CASE(GL_BOOL, 1, GLboolean);
            CASE(GL_BOOL_VEC2, 2, GLboolean);
            CASE(GL_BOOL_VEC3, 3, GLboolean);
            CASE(GL_BOOL_VEC4, 4, GLboolean);
            CASE(GL_FLOAT_MAT2, 4, GLfloat);
            CASE(GL_FLOAT_MAT2x3, 6, GLfloat);
            CASE(GL_FLOAT_MAT2x4, 8, GLfloat);
            CASE(GL_FLOAT_MAT3, 9, GLfloat);
            CASE(GL_FLOAT_MAT3x2, 6, GLfloat);
            CASE(GL_FLOAT_MAT3x4, 12, GLfloat);
            CASE(GL_FLOAT_MAT4, 16, GLfloat);
            CASE(GL_FLOAT_MAT4x2, 8, GLfloat);
            CASE(GL_FLOAT_MAT4x3, 12, GLfloat);
#undef CASE
            default:
                fprintf(stderr, "Unknown type: 0x%x\n", type);
                break;
        }

        return size;
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
        //TODO: create a meta function which would enumerate or uniform types
        template<typename TUniform>
        requires(std::is_fundamental_v<std::remove_reference_t<TUniform>>
            || std::is_same_v<std::remove_reference_t<TUniform>, math::Vector4f>
            || std::is_same_v<std::remove_reference_t<TUniform>, math::Matrix4x4f>)
        static void process(const std::string_view& memberName, const std::size_t uniformId, const TUniform value, OpenglMaterialBinding& material)
        {
            GLint location = glGetUniformLocation(material.glProgramId, memberName.data());
            if(location >= 0)
            {
                material.uniformLocations[uniformId] = location;
            }
        }

        template<typename TUniform>
        requires(std::is_class_v<std::decay_t<TUniform>>
                 && !std::is_same_v<std::remove_reference_t<TUniform>, math::Vector4f>
                 && !std::is_same_v<std::remove_reference_t<TUniform>, math::Matrix4x4f>)
        static void process(const std::string_view& memberName, const std::size_t uniformId, const TUniform& value, OpenglMaterialBinding& material)
        {
            //TODO: test and debug multiple blocks per program, as well as named blocks(currently it is crushing)

            UniformBlockBinding blockBinding;
            blockBinding.blockIndex = glGetUniformBlockIndex(material.glProgramId, memberName.data());
            if(blockBinding.blockIndex != GL_INVALID_INDEX)
            {
                glGetActiveUniformBlockiv(material.glProgramId, blockBinding.blockIndex, GL_UNIFORM_BLOCK_DATA_SIZE, &blockBinding.blockSize);
                if(blockBinding.blockSize > 0)
                {
                    blockBinding.bufferOffset = material.uniformBlockBufferSize;
                    material.uniformBlockBufferSize += blockBinding.blockSize;

                    const constexpr auto N = reflection::MemberCountV<TUniform>;
                    const constexpr auto Names = reflection::MemberNamesV<TUniform>;

                    blockBinding.memberIndices = std::vector<GLuint>(N);
                    blockBinding.memberCounts = std::vector<GLint>(N);
                    blockBinding.memberOffsets = std::vector<GLint>(N);
                    blockBinding.memberTypes = std::vector<GLint>(N);

                    auto indices = blockBinding.memberIndices.data();

                    glGetUniformIndices(material.glProgramId, N, Names, indices);
                    glGetActiveUniformsiv(material.glProgramId, N, indices, GL_UNIFORM_OFFSET, blockBinding.memberOffsets.data());
                    glGetActiveUniformsiv(material.glProgramId, N, indices, GL_UNIFORM_SIZE, blockBinding.memberCounts.data());
                    glGetActiveUniformsiv(material.glProgramId, N, indices, GL_UNIFORM_TYPE, blockBinding.memberTypes.data());

                    material.uniformBlocks.emplace(uniformId, blockBinding);
                }
            }
        }
    };

    void bindUniformLocations(OpenglMaterialBinding* materialBinding)
    {
        //TODO: think about how to bind MaterialProperties only once for a program, because they are never changing(static uniforms)
        //instead of Uniforms{} use Uniforms with material properties and other const uniforms set
        services::graphics::UniformVisitor<UniformLocationBinder>::visit(Uniforms{}, *materialBinding);

        if(materialBinding->uniformBlockBufferSize > 0)
        {
            glCreateBuffers(1, &materialBinding->uniformBlockBufferId);
            glNamedBufferStorage(materialBinding->uniformBlockBufferId, (GLsizeiptr)materialBinding->uniformBlockBufferSize, nullptr, GL_MAP_WRITE_BIT);
            for(const auto& [key, blockBinding] : materialBinding->uniformBlocks)
            {
                glBindBufferRange(GL_UNIFORM_BUFFER, blockBinding.blockIndex, materialBinding->uniformBlockBufferId, (GLintptr)blockBinding.bufferOffset, blockBinding.blockSize);
            }
        }
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

    struct UniformBlockValueContext
    {
        void* buffer;
        const UniformBlockBinding& block;
    };

    template<typename T>
    void writeToBuffer(void* buffer, std::size_t& offset, const T& value)
    {
        memcpy((char*)buffer + offset, &value, sizeof(value));
        offset += sizeof(value);
    }

    class UniformBlockValueBinder
    {
    public:
        static void process(const std::string_view& memberName, const std::size_t memberId, const math::Vector4f& value, UniformBlockValueContext& context)
        {
            std::size_t offset = context.block.memberOffsets[memberId];

            writeToBuffer(context.buffer, offset, value.x);
            writeToBuffer(context.buffer, offset, value.y);
            writeToBuffer(context.buffer, offset, value.z);
            writeToBuffer(context.buffer, offset, value.w);
        }
    };

    class UniformValueBinder
    {
    public:
        static void process(const std::string_view& memberName, const std::size_t uniformId, const math::Vector4f& value, OpenglMaterialBinding& material)
        {
            if(material.uniformLocations.contains(uniformId))
            {
                auto location = material.uniformLocations[uniformId];
                glUniform4f(location, value.x, value.y, value.z, value.w);
            }
        }

        static void process(const std::string_view& memberName, const std::size_t uniformId, const math::Matrix4x4f& value, OpenglMaterialBinding& material)
        {
            if(material.uniformLocations.contains(uniformId))
            {
                auto location = material.uniformLocations[uniformId];
                glUniformMatrix4fv(location, 1, false, value.entries.data());
            }
        }

        template<typename TUniform>
        static void process(const std::string_view& memberName, const std::size_t uniformId, const TUniform& value, OpenglMaterialBinding& material)
        {
            if(material.uniformBlocks.contains(uniformId))
            {
                auto block = material.uniformBlocks[uniformId];
                void* buffer = glMapNamedBuffer(material.uniformBlockBufferId, GL_WRITE_ONLY);

                auto context = UniformBlockValueContext{ (char*)buffer + block.bufferOffset, block };
                services::graphics::UniformVisitor<UniformBlockValueBinder>::visit(value, context);

                glUnmapNamedBuffer(material.uniformBlockBufferId);
            }
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
            uniforms.material =  materialBinding->properties;
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