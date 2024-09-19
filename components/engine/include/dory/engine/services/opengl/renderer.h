#pragma once

#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <dory/engine/resources/opengl/blocks.h>
#include <dory/engine/resources/opengl/program.h>
#include <dory/engine/resources/opengl/vertexArray.h>
#include <dory/engine/services/opengl/openglService.h>
#include <dory/engine/services/opengl/shaderService.h>

namespace dory::engine::services::opengl
{
    struct TrianglesVertexArray: public resources::opengl::AttributedVertexArray<1>
    {
        TrianglesVertexArray():
                resources::opengl::AttributedVertexArray<1>({resources::opengl::VertexAttribute(2, GL_FLOAT)})
        {}
    };

    struct ColorsBufferInterface
    {
        resources::Color brightColor;
        resources::Color hippieColor;
        resources::Color darkColor;

        ColorsBufferInterface(const resources::Color& brightColor, const resources::Color& hippieColor, const resources::Color& darkColor):
            brightColor(brightColor), hippieColor(hippieColor), darkColor(darkColor)
        {}

    };

    struct ColorsUniformBlock: public resources::opengl::ArrayUniformBlock<3>
    {
        static constexpr std::string_view pointLiteral = ".";
        static constexpr std::string_view blockNameLiteral = "ColorsBlock";
        static constexpr std::string_view prefixLiteral = dory::JoinStringLiterals<blockNameLiteral, pointLiteral>;

        static constexpr std::string_view brightColorLiteral = "brightColor";
        static constexpr std::string_view hippieColorLiteral = "hippieColor";
        static constexpr std::string_view darkColorLiteral = "darkColor";

        ColorsUniformBlock():
                resources::opengl::ArrayUniformBlock<3>(blockNameLiteral,
                dory::JoinStringLiterals<prefixLiteral, brightColorLiteral>,
                dory::JoinStringLiterals<prefixLiteral, hippieColorLiteral>,
                dory::JoinStringLiterals<prefixLiteral, darkColorLiteral>)
        {}
    };

    struct TrianglesProgram: resources::opengl::Program
    {
        ColorsUniformBlock colorsUniformBlock;

        TrianglesProgram():
            resources::opengl::Program("Triangles Program",
            {
                resources::opengl::Shader(GL_VERTEX_SHADER, "shaders/triangles/triangles.vert"),
                resources::opengl   ::Shader(GL_FRAGMENT_SHADER, "shaders/triangles/triangles.frag")
            })
        {}
    };

    template<typename TImplementation, typename TDataContext>
    class IRenderer: public StaticInterface<TImplementation>
    {
    public:
        void initialize(TDataContext& dataContext)
        {
             this->toImplementation()->initializeImpl(dataContext);
        }

        void draw()
        {
            this->toImplementation()->drawImpl();
        }
    };

    template<typename TDataContext, typename T>
    class RendererFactory;

    template<typename TShaderService>
    struct RendererDependencies
    {
        using ShaderServiceType = TShaderService;
    };

    template<typename TDataContext, typename T>
    requires(is_instance_v<T, RendererDependencies>)
    class Renderer: public IRenderer<Renderer<TDataContext, T>, TDataContext>
    {
    private:
        using ShaderServiceType = IShaderService<typename T::ShaderServiceType, TDataContext>;
        ShaderServiceType& shaderService;

        TrianglesProgram trianglesProgram;
        TrianglesVertexArray trianglesVertexArray;
        resources::Color clearScreenColor {0.0f, 0.0f, 0.0f};

        float colorDelta = 0.0001f;
        float hippieColorDelta = colorDelta;

        ColorsBufferInterface colorsUniformData {
                resources::Color(0.7f, 0.7f, 0.7f, 1.0f),
                resources::Color(0.2f, 0.2, 0.f, 1.0f),
                resources::Color(0.2f, 0.2f, 0.2f, 1.0f)
        };

        std::array<GLfloat, 12> verticesData = {
                -0.90f, -0.90f,
                0.85f, -0.90f,
                -0.90f,  0.85f,

                0.90f, -0.85f,
                0.90f,  0.90f,
                -0.85f,  0.90f
        };

    public:
        using FactoryType = RendererFactory<TDataContext, T>;

        explicit Renderer(ShaderServiceType& shaderService):
                shaderService(shaderService)
        {}

        void initializeImpl(TDataContext& dataContext)
        {
            shaderService.loadProgram(dataContext, trianglesProgram, [](ShaderServiceError& error)
            {
                if(error.shaderCompilationError)
                {
                    std::cerr << "Shader compilation error(" << error.shaderCompilationError->shaderIdentifier << "): "
                              << error.shaderCompilationError->compilationLog << std::endl;
                }
                else if(error.shaderProgramLinkingError)
                {
                    std::cerr << "Shader program linking error: " << error.shaderProgramLinkingError->linkingLog << std::endl;
                }
            });

            OpenglService::loadUniformBlock(trianglesProgram.id, trianglesProgram.colorsUniformBlock);
            OpenglService::setUniformBlockData(trianglesProgram.colorsUniformBlock, &colorsUniformData, sizeof(colorsUniformData));

            OpenglService::loadVertexArray(trianglesVertexArray);
            OpenglService::setVertexArrayData(trianglesVertexArray, verticesData.data(), sizeof(verticesData));
        }

        void drawImpl()
        {
            OpenglService::clearViewport(clearScreenColor);

            auto& hippieColor = colorsUniformData.hippieColor;

            if(hippieColor.r >= 1.f || hippieColor.g  >= 1.f || hippieColor.b  >= 1.f)
            {
                hippieColorDelta = -colorDelta;
            }
            else if(hippieColor.r <= 0.f || hippieColor.g <= 0.f || hippieColor.b <= 0.f)
            {
                hippieColorDelta = colorDelta;
            }

            hippieColor.r += hippieColorDelta;
            hippieColor.g += hippieColorDelta;
            hippieColor.b += hippieColorDelta;

            OpenglService::setUniformBlockData(trianglesProgram.colorsUniformBlock, &colorsUniformData, sizeof(colorsUniformData));

            OpenglService::drawObject(trianglesProgram, trianglesVertexArray);
        }
    };

    template<typename TDataContext, typename T>
    class RendererFactory: public IServiceFactory<RendererFactory<TDataContext, T>>
    {
    private:
        using ShaderServiceType = IShaderService<typename T::ShaderServiceType, TDataContext>;
        ShaderServiceType& shaderService;

    public:
        explicit RendererFactory(ShaderServiceType& shaderService):
                shaderService(shaderService)
        {}

        auto createInstanceImpl()
        {
            return Renderer<TDataContext, T>{shaderService};
        }
    };
}