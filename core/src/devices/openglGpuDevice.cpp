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
#include "dory/core/services/graphics/openglUniformBinder.h"
#include <iostream>

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

    std::optional<const std::string> getCurrentGlError(const std::string& location)
    {
        GLenum errorCode = glGetError();
        if (errorCode != GL_NO_ERROR)
        {
            return fmt::format("OpenGL error at location: {0}, {1}", location, getErrorString(errorCode));
        }

        return {};
    }

    std::size_t getGlTypeSize(GLenum type)
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

    template<typename T>
    void writeToBuffer(void* buffer, std::size_t& offset, const T& value)
    {
        memcpy((char*)buffer + offset, &value, sizeof(value));
        offset += sizeof(value);
    }

    bool isUniformBlockInstanceNamed(GLuint programId, GLuint blockId)
    {
        GLint numUniforms = 0;
        glGetActiveUniformBlockiv(programId, blockId, GL_UNIFORM_BLOCK_ACTIVE_UNIFORMS, &numUniforms);

        std::vector<GLint> uniformIndices(numUniforms);
        glGetActiveUniformBlockiv(programId, blockId, GL_UNIFORM_BLOCK_ACTIVE_UNIFORM_INDICES, uniformIndices.data());

        GLint maxNameLength = 0;
        glGetProgramiv(programId, GL_ACTIVE_UNIFORM_MAX_LENGTH, &maxNameLength);

        bool hasInstanceName = false;
        std::string instanceName;

        for (GLint i = 0; i < numUniforms; ++i)
        {
            std::vector<char> nameBuffer(maxNameLength);
            GLsizei length = 0;
            glGetActiveUniformName(programId, uniformIndices[i], maxNameLength, &length, nameBuffer.data());

            std::string uniformName(nameBuffer.data(), length);

            size_t dotPos = uniformName.find('.');
            if (dotPos != std::string::npos)
            {
                return true;
            }
        }

        return false;
    }

    using UniformTypes = generic::TypeList<math::Vector4f, math::Matrix4x4f>;

    template<typename TUniform>
    static constexpr const bool IsUniform = generic::IsInTypeListV<std::remove_reference_t<TUniform>, UniformTypes> || std::is_fundamental_v<std::decay_t<TUniform>>;

    struct UniformBindingContext
    {
        const OpenglMaterialBinding& material;
        const OpenglProperties& openglProperties;
        const Registry& registry;
        UniformBinding& uniforms;

        unsigned int uniformBlockCount {};
    };

    class UniformLocationBinder
    {
    public:
        template<typename TUniform>
        requires(IsUniform<TUniform>)
        static void process(const std::string_view& memberName, const std::size_t uniformId, const TUniform value, UniformBindingContext& context)
        {
            const OpenglMaterialBinding& material = context.material;
            GLint location = glGetUniformLocation(material.glProgramId, memberName.data());
            if(location >= 0)
            {
                context.uniforms.locations[uniformId] = location;
            }
        }

        template<typename TUniform>
        requires(!IsUniform<TUniform>)
        static void process(const std::string_view& memberName, const std::size_t uniformId, const TUniform& value, UniformBindingContext& context)
        {
            static constexpr auto typeName = reflection::getTypeSimpleName<TUniform>();
            const OpenglMaterialBinding& material = context.material;

            UniformBlockBinding blockBinding;
            blockBinding.blockIndex = glGetUniformBlockIndex(material.glProgramId, typeName.data());
            if(blockBinding.blockIndex != GL_INVALID_INDEX)
            {
                glGetActiveUniformBlockiv(material.glProgramId, blockBinding.blockIndex, GL_UNIFORM_BLOCK_DATA_SIZE, &blockBinding.blockSize);
                if(blockBinding.blockSize > 0)
                {
                    blockBinding.bufferOffset = context.uniforms.blockBufferSize;
                    blockBinding.bindingPoint = context.uniformBlockCount;

                    GLint alignment = context.openglProperties.bufferAlignment;
                    glGetIntegerv(GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT, &alignment);
                    if (blockBinding.blockSize % alignment != 0)
                    {
                        blockBinding.blockSize = ((blockBinding.blockSize / alignment) + 1) * alignment;
                    }
                    context.uniforms.blockBufferSize += blockBinding.blockSize;

                    const constexpr auto N = reflection::MemberCountV<TUniform>;
                    const constexpr auto Names = reflection::MemberNamesV<TUniform>;
                    const constexpr auto PrefixedNames = reflection::PrefixedMemberNamesV<TUniform>;
                    const char* const * uniformNames = isUniformBlockInstanceNamed(material.glProgramId, blockBinding.blockIndex) ? PrefixedNames : Names;

                    blockBinding.memberIndices = std::vector<GLuint>(N);
                    blockBinding.memberCounts = std::vector<GLint>(N);
                    blockBinding.memberOffsets = std::vector<GLint>(N);
                    blockBinding.memberTypes = std::vector<GLint>(N);

                    auto indices = blockBinding.memberIndices.data();

                    glGetUniformIndices(material.glProgramId, N, uniformNames, indices);

                    glGetActiveUniformsiv(material.glProgramId, N, indices, GL_UNIFORM_OFFSET, blockBinding.memberOffsets.data());
                    glGetActiveUniformsiv(material.glProgramId, N, indices, GL_UNIFORM_SIZE, blockBinding.memberCounts.data());
                    glGetActiveUniformsiv(material.glProgramId, N, indices, GL_UNIFORM_TYPE, blockBinding.memberTypes.data());

                    context.uniforms.blocks[uniformId] = blockBinding;
                    ++context.uniformBlockCount;
                }
            }
        }
    };

    struct UniformBlockValueContext
    {
        void* buffer;
        const UniformBlockBinding& block;
    };

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
        static void process(const std::string_view& memberName, const std::size_t uniformId, const math::Vector4f& value, UniformBindingContext& context)
        {
            if(context.uniforms.locations.contains(uniformId))
            {
                auto location = context.uniforms.locations[uniformId];
                glUniform4f(location, value.x, value.y, value.z, value.w);
            }
        }

        static void process(const std::string_view& memberName, const std::size_t uniformId, const math::Matrix4x4f& value, UniformBindingContext& context)
        {
            if(context.uniforms.locations.contains(uniformId))
            {
                auto location = context.uniforms.locations[uniformId];
                glUniformMatrix4fv(location, 1, false, value.entries.data());
            }
        }

        template<typename TUniform>
        requires(!IsUniform<TUniform>)
        static void process(const std::string_view& memberName, const std::size_t uniformId, const TUniform& value, UniformBindingContext& context)
        {
            if(context.uniforms.blocks.contains(uniformId))
            {
                auto block = context.uniforms.blocks[uniformId];
                void* buffer = glMapNamedBuffer(context.uniforms.blockBufferId, GL_WRITE_ONLY);

                auto blockContext = UniformBlockValueContext{ (char*)buffer + block.bufferOffset, block };
                services::graphics::UniformVisitor<UniformBlockValueBinder>::visit(value, blockContext);

                glUnmapNamedBuffer(context.uniforms.blockBufferId);
            }
        }
    };

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

        glGetIntegerv(GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT, &openglProperties.bufferAlignment);
    }

    void OpenglGpuDevice::disconnect(DataContext& context)
    {}

    bool OpenglGpuDevice::checkForError(const std::string& location)
    {
        auto errorMessage = getCurrentGlError(location);
        if(errorMessage)
        {
            _registry.get<ILogService>([&errorMessage](ILogService* logger) {
                logger->error(*errorMessage);
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

        return !checkForError("allocateBuffer");
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
        checkForError("writeData to buffer");
    }

    void OpenglGpuDevice::bindMesh(MeshBinding* meshBinding, const BufferBinding* vertexBuffer, const BufferBinding* indexBuffer)
    {
        auto glMesh = (OpenglMeshBinding*)meshBinding;

        if(glMesh->glVertexArrayId == 0 && !glIsVertexArray(glMesh->glVertexArrayId))
        {
            glCreateVertexArrays(1, &glMesh->glVertexArrayId);
            checkForError("create vertex array");
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

        checkForError("bindMesh");
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
            bindUniforms(glMaterial);
        }
    }

    void OpenglGpuDevice::drawFrame(const Frame& frame)
    {
        glClearColor(frame.clearColor.x, frame.clearColor.y, frame.clearColor.z, frame.clearColor.w);
        glClear(GL_COLOR_BUFFER_BIT);

        DynamicUniforms uniforms;

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

    template<typename TUniform>
    void OpenglGpuDevice::bindUniformLocations(OpenglMaterialBinding* materialBinding, UniformBinding& uniforms)
    {
        auto bindingContext = UniformBindingContext
                {
                        *materialBinding,
                        openglProperties,
                        _registry,
                        uniforms
                };
        services::graphics::UniformVisitor<UniformLocationBinder>::visit(TUniform{}, bindingContext);

        if(uniforms.blockBufferSize > 0)
        {
            glCreateBuffers(1, &uniforms.blockBufferId);
            glNamedBufferStorage(uniforms.blockBufferId, (GLsizeiptr)uniforms.blockBufferSize, nullptr, GL_MAP_WRITE_BIT);

            for(auto& [key, blockBinding] : uniforms.blocks)
            {
                glBindBufferRange(GL_UNIFORM_BUFFER, blockBinding.blockIndex, uniforms.blockBufferId, (GLintptr)blockBinding.bufferOffset, blockBinding.blockSize);
                checkForError("bind uniform block");
                glUniformBlockBinding(bindingContext.material.glProgramId, blockBinding.blockIndex, blockBinding.bindingPoint);
            }
        }
    }

    void OpenglGpuDevice::bindUniforms(OpenglMaterialBinding* materialBinding)
    {
        bindUniformLocations<DynamicUniforms>(materialBinding, materialBinding->dynamicUniforms);
        bindUniformLocations<StaticUniforms>(materialBinding, materialBinding->staticUniforms);

        StaticUniforms uniforms;
        fillUniforms(uniforms, materialBinding);

        auto bindingContext = UniformBindingContext
                {
                        *materialBinding,
                        openglProperties,
                        _registry,
                        materialBinding->staticUniforms,
                };

        services::graphics::UniformVisitor<UniformValueBinder>::visit(uniforms, bindingContext);
    }

    void OpenglGpuDevice::setActiveMaterial(const DynamicUniforms& uniforms, const MaterialBinding* materialBinding)
    {
        auto glMaterial = (OpenglMaterialBinding*)materialBinding;

        if(glMaterial && glMaterial->linkingError.empty())
        {
            assert(glIsProgram(glMaterial->glProgramId));
            glUseProgram(glMaterial->glProgramId);

            auto bindingContext = UniformBindingContext
                    {
                            *glMaterial,
                            openglProperties,
                            _registry,
                            glMaterial->dynamicUniforms,
                    };

            services::graphics::UniformVisitor<UniformValueBinder>::visit(uniforms, bindingContext);
        }
    }

    void OpenglGpuDevice::fillUniforms(DynamicUniforms& uniforms, const MaterialBinding* materialBinding)
    {
        if(materialBinding)
        {
        }
    }

    void OpenglGpuDevice::fillUniforms(StaticUniforms& uniforms, const MaterialBinding* materialBinding)
    {
        if(materialBinding)
        {
            uniforms.material =  materialBinding->properties;
        }
    }

    void OpenglGpuDevice::drawMesh(const MeshBinding* meshBinding)
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
}