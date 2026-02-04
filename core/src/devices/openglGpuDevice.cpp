#include "dory/core/registry.h"
#include "dory/core/devices/openglGpuDevice.h"
#ifndef DORY_OPENGL_INCLUDED
#include <glad/gl.h>
#define DORY_OPENGL_INCLUDED
#endif
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
#include <ranges>

namespace dory::core::devices
{
    using namespace resources;
    using namespace resources::bindings;
    using namespace resources::bindings::uniforms;
    using namespace resources::objects;
    using namespace services;

    bool getFrameBufferPixels(const GLenum buffer, const entities::View& view, assets::Image& image)
    {
        // Bind the specified buffer
        glReadBuffer(buffer);

        image.width = view.viewport.width;
        image.height = view.viewport.height;
        image.components = 4;

        // Allocate memory to store the pixels (RGBA format)
        image.data = std::vector<unsigned char>(image.width  * image.height * 4);

        // Read the pixels from the buffer
        glReadPixels(0, 0, image.width, image.height, GL_RGBA, GL_UNSIGNED_BYTE, image.data.data());

        // Flip the image vertically (OpenGL stores it upside down)
        for (int y = 0; y < image.height / 2; ++y) {
            for (int x = 0; x < image.width * 4; ++x) {
                std::swap(image.data[y * image.width * 4 + x], image.data[(image.height - y - 1) * image.width * 4 + x]);
            }
        }

        return true;
    }

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
        return 0;
    }

    std::size_t getComponentSize(const ComponentType componentType)
    {
        switch (componentType)
        {
            case ComponentType::floatType: return sizeof(float );
            case ComponentType::doubleType: return sizeof(double );
            case ComponentType::uintType: return sizeof(unsigned int);
        }

        assert(false);
        return 0;
    }

    GLenum getGlShaderType(const resources::assets::ShaderType type)
    {
        switch (type)
        {
            case resources::assets::ShaderType::vertex: return GL_VERTEX_SHADER;
            case resources::assets::ShaderType::fragment: return GL_FRAGMENT_SHADER;
        }

        assert(false);
        return 0;
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

        for (GLint i = 0; i < numUniforms; ++i)
        {
            std::vector<char> nameBuffer(maxNameLength);
            GLsizei length = 0;
            glGetActiveUniformName(programId, uniformIndices[i], maxNameLength, &length, nameBuffer.data());

            std::string uniformName(nameBuffer.data(), length);

            std::size_t dotPos = uniformName.find('.');
            if (dotPos != std::string::npos)
            {
                return true;
            }
        }

        return false;
    }

    using UniformTypes = generic::TypeList<math::Vector4f, math::Matrix4x4f, glm::mat4x4, glm::vec3>;

    template<typename TUniform>
    static constexpr bool IsUniform = generic::IsInTypeListV<std::remove_reference_t<TUniform>, UniformTypes> || std::is_fundamental_v<std::decay_t<TUniform>>;

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
            const OpenglMaterialBinding& material = context.material;

            UniformBlockBinding blockBinding;
            constexpr auto typeName = reflection::getTypeSimpleName<TUniform>();
            blockBinding.blockIndex = glGetUniformBlockIndex(material.glProgramId, typeName.data);
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

                    constexpr auto N = reflection::MemberCountV<TUniform>;
                    constexpr auto Names = reflection::MemberNamesV<TUniform>;
                    constexpr auto PrefixedNames = reflection::PrefixedMemberNamesV<TUniform>;
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

        static void process(const std::string_view& memberName, const std::size_t uniformId, const glm::mat4x4 value, UniformBindingContext& context)
        {
            if(context.uniforms.locations.contains(uniformId))
            {
                auto location = context.uniforms.locations[uniformId];
                glUniformMatrix4fv(location, 1, false, glm::value_ptr(value));
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

    static void glfwErrorCallback(int code, const char* desc)
    {
        fprintf(stderr, "GLFW error %d: %s\n", code, desc);
    }

    void OpenglGpuDevice::connect(DataContext& context)
    {
        glfwSetErrorCallback(glfwErrorCallback);
        glfwInit();
        glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
#if defined(DORY_PLATFORM_APPLE)
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
        GLFWwindow* hidden_window = glfwCreateWindow(1, 1, "", NULL, NULL);
        glfwMakeContextCurrent(hidden_window);
        int version = gladLoadGL(glfwGetProcAddress);
        if (version == 0)
        {
            auto logger = _registry.get<ILogService, Logger::App>();
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

    bool OpenglGpuDevice::checkForError(const std::string& location) const
    {
        auto errorMessage = getCurrentGlError(location);
        if(errorMessage)
        {
            _registry.get<ILogService, Logger::App>([&errorMessage](ILogService* logger) {
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
            for(const auto& attribute : glMesh->vertexAttributes)
            {
                std::size_t stride = attribute.componentsCount * getComponentSize(attribute.componentType);
                glVertexArrayVertexBuffer(glMesh->glVertexArrayId, i, glVertexBuffer->glId,
                        (GLintptr)(attribute.offset + glMesh->vertexBufferOffset), (GLsizei)stride);
                glVertexArrayAttribFormat(glMesh->glVertexArrayId, i, (GLint)attribute.componentsCount,
                        getGlType(attribute.componentType), attribute.normalized, 0);
                glVertexArrayAttribBinding(glMesh->glVertexArrayId, i, i);
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

    void OpenglGpuDevice::drawFrame(const Frame& frame, profiling::Profiling& profiling)
    {
        resources::profiling::pushTimeSlice(profiling, "OpenglGpuDevice::drawFrame - setup");

        /*if(auto* currentFrame = profiling::getCurrentFrame(profiling))
        {
            GLint framebufferID;
            glGetIntegerv(GL_FRAMEBUFFER_BINDING, &framebufferID);
            currentFrame->frameBufferBinding = framebufferID;

            GLint readBuffer;
            glGetIntegerv(GL_READ_BUFFER, &readBuffer);
            currentFrame->readFrameBufferIndex = readBuffer == GL_BACK;
            GLint drawBuffer;
            glGetIntegerv(GL_DRAW_BUFFER, &drawBuffer);
            currentFrame->drawFrameBufferIndex = drawBuffer == GL_BACK;
        }*/

        resources::profiling::pushTimeSlice(profiling, "OpenglGpuDevice::drawFrame - enable depth test");
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);
        resources::profiling::popTimeSlice(profiling); //enable depth test

        glClearColor(frame.clearColor.x, frame.clearColor.y, frame.clearColor.z, frame.clearColor.w);

        resources::profiling::pushTimeSlice(profiling, "OpenglGpuDevice::drawFrame - clear buffers");
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
        resources::profiling::popTimeSlice(profiling); //clear buffers

        glViewport((GLint)frame.viewport.x, (GLint)frame.viewport.y, (GLint)frame.viewport.width, (GLint)frame.viewport.height);

        DynamicUniforms uniforms;
        ModelUniforms modelUniforms;

        uniforms.viewProjectionTransform = frame.viewProjectionTransform;

        resources::profiling::popTimeSlice(profiling); //setup

        resources::profiling::pushTimeSlice(profiling, "OpenglGpuDevice::drawFrame - draw meshes");

        for(const auto& [material, meshes] : frame.meshMap)
        {
            fillUniforms(uniforms, material);
            setActiveMaterial(uniforms, material);
            for(const auto& meshItem : meshes)
            {
                modelUniforms.modelTransform = meshItem.modelTransform;
                setModelUniforms(modelUniforms, material);
                drawMesh(meshItem.meshBinding);
            }
        }

        resources::profiling::popTimeSlice(profiling); //"OpenglGpuDevice::drawFrame - draw meshes"
    }

    void OpenglGpuDevice::completeFrame(const Frame& frame, profiling::Profiling& profiling)
    {
        /*resources::profiling::pushTimeSlice(profiling, "OpenglGpuDevice::completeFrame - glFinish", std::chrono::steady_clock::now());
        glFinish();
        resources::profiling::popTimeSlice(profiling,  std::chrono::steady_clock::now()); //glFinish*/
    }

    bool OpenglGpuDevice::getFrontBufferImage(const entities::View& view, assets::Image& image)
    {
        return getFrameBufferPixels(GL_FRONT, view, image);
    }

    bool OpenglGpuDevice::getBackBufferImage(const entities::View& view, assets::Image& image)
    {
        return getFrameBufferPixels(GL_BACK, view, image);
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

            for(const auto& blockBinding : uniforms.blocks | std::views::values)
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
        bindUniformLocations<ModelUniforms>(materialBinding, materialBinding->modelUniforms);

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

    void OpenglGpuDevice::setModelUniforms(const ModelUniforms& uniforms, const MaterialBinding* materialBinding) const
    {
        auto glMaterial = (OpenglMaterialBinding*)materialBinding;

        auto bindingContext = UniformBindingContext
                {
                        *glMaterial,
                        openglProperties,
                        _registry,
                        glMaterial->modelUniforms,
                };

        services::graphics::UniformVisitor<UniformValueBinder>::visit(uniforms, bindingContext);
    }

    void OpenglGpuDevice::setActiveMaterial(const DynamicUniforms& uniforms, const MaterialBinding* materialBinding) const
    {
        auto glMaterial = (OpenglMaterialBinding*)materialBinding;

        if(glMaterial)
        {
            if(glMaterial->linkingError.empty())
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

            GLenum faceMode = GL_FILL;
            if(glMaterial->material.polygonMode == resources::assets::PolygonMode::Wireframe)
            {
                faceMode = GL_LINE;
                glDisable(GL_CULL_FACE);

                //TODO: move multisampling settings to material
                glEnable(GL_MULTISAMPLE);
                glEnable(GL_LINE_SMOOTH);
            }
            else if(glMaterial->material.polygonMode == resources::assets::PolygonMode::Solid)
            {
                glEnable(GL_CULL_FACE);
                glCullFace(GL_BACK);
            }

            glPolygonMode(GL_FRONT_AND_BACK, faceMode);
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
            uniforms.material =  materialBinding->material.properties;
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
