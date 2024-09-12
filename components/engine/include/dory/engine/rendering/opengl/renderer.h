#pragma once

#include "dory/engine/resources/opengl/blocks.h"
#include "dory/engine/resources/opengl/program.h"
#include "dory/engine/resources/opengl/vertexArray.h"
#include "services/openglService.h"
#include "services/shaderService.h"

namespace dory::opengl
{
    struct TrianglesVertexArray: public graphics::AttributedVertexArray<1>
    {
        TrianglesVertexArray():
            graphics::AttributedVertexArray<1>({graphics::VertexAttribute(2, GL_FLOAT)})
        {}
    };

    struct ColorsBufferInterface
    {
        domain::Color brightColor;
        domain::Color hippieColor;
        domain::Color darkColor;

        ColorsBufferInterface(const domain::Color& brightColor, const domain::Color& hippieColor, const domain::Color& darkColor):
            brightColor(brightColor), hippieColor(hippieColor), darkColor(darkColor)
        {}

    };

    struct ColorsUniformBlock: public graphics::ArrayUniformBlock<3>
    {
        static constexpr std::string_view pointLiteral = ".";
        static constexpr std::string_view blockNameLiteral = "ColorsBlock";
        static constexpr std::string_view prefixLiteral = dory::JoinStringLiterals<blockNameLiteral, pointLiteral>;

        static constexpr std::string_view brightColorLiteral = "brightColor";
        static constexpr std::string_view hippieColorLiteral = "hippieColor";
        static constexpr std::string_view darkColorLiteral = "darkColor";

        ColorsUniformBlock():
            graphics::ArrayUniformBlock<3>(blockNameLiteral, 
                dory::JoinStringLiterals<prefixLiteral, brightColorLiteral>,
                dory::JoinStringLiterals<prefixLiteral, hippieColorLiteral>,
                dory::JoinStringLiterals<prefixLiteral, darkColorLiteral>)
        {}
    };

    struct TrianglesProgram: graphics::Program
    {
        ColorsUniformBlock colorsUniformBlock;

        TrianglesProgram():
            graphics::Program("Triangles Program",
            { 
                graphics::Shader(GL_VERTEX_SHADER, "shaders/triangles/triangles.vert"), 
                graphics::Shader(GL_FRAGMENT_SHADER, "shaders/triangles/triangles.frag") 
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
        using ShaderServiceType = services::IShaderService<typename T::ShaderServiceType, TDataContext>;
        ShaderServiceType& shaderService;

        TrianglesProgram trianglesProgram;
        TrianglesVertexArray trianglesVertexArray;
        domain::Color clearScreenColor {0.0f, 0.0f, 0.0f};

        float colorDelta = 0.0001f;
        float hippieColorDelta = colorDelta;

        ColorsBufferInterface colorsUniformData {
                domain::Color(0.7f, 0.7f, 0.7f, 1.0f),
                domain::Color(0.2f, 0.2, 0.f, 1.0f),
                domain::Color(0.2f, 0.2f, 0.2f, 1.0f)
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
            shaderService.loadProgram(dataContext, trianglesProgram, [](services::ShaderServiceError& error)
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

            services::OpenglService::loadUniformBlock(trianglesProgram.id, trianglesProgram.colorsUniformBlock);
            services::OpenglService::setUniformBlockData(trianglesProgram.colorsUniformBlock, &colorsUniformData, sizeof(colorsUniformData));

            services::OpenglService::loadVertexArray(trianglesVertexArray);
            services::OpenglService::setVertexArrayData(trianglesVertexArray, verticesData.data(), sizeof(verticesData));
        }

        void drawImpl()
        {
            services::OpenglService::clearViewport(clearScreenColor);

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

            services::OpenglService::setUniformBlockData(trianglesProgram.colorsUniformBlock, &colorsUniformData, sizeof(colorsUniformData));

            services::OpenglService::drawObject(trianglesProgram, trianglesVertexArray);
        }
    };

    template<typename TDataContext, typename T>
    class RendererFactory: public IServiceFactory<RendererFactory<TDataContext, T>>
    {
    private:
        using ShaderServiceType = services::IShaderService<typename T::ShaderServiceType, TDataContext>;
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