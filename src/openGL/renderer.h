#pragma once

#include "dependencies.h"
#include "base/base.h"
#include "graphics/program.h"
#include "graphics/blocks.h"
#include "graphics/vertexArray.h"
#include "services/openglService.h"
#include "services/shaderService.h"

namespace dory::openGL
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
        static constexpr std::string_view prefixLiteral = dory::compileTime::JoinStringLiterals<blockNameLiteral, pointLiteral>;

        static constexpr std::string_view brightColorLiteral = "brightColor";
        static constexpr std::string_view hippieColorLiteral = "hippieColor";
        static constexpr std::string_view darkColorLiteral = "darkColor";

        ColorsUniformBlock():
            graphics::ArrayUniformBlock<3>(blockNameLiteral, 
                dory::compileTime::JoinStringLiterals<prefixLiteral, brightColorLiteral>, 
                dory::compileTime::JoinStringLiterals<prefixLiteral, hippieColorLiteral>, 
                dory::compileTime::JoinStringLiterals<prefixLiteral, darkColorLiteral>)
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

    template<typename TServiceLocator>
    class Renderer: public domain::Service<TServiceLocator>
    {
    private:
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
        explicit Renderer(TServiceLocator& serviceLocator):
            domain::Service<TServiceLocator>(serviceLocator)
        {
        }

        void initialize()
        {
            services::ShaderService::loadProgram(this->services, trianglesProgram, [](services::ShaderServiceError& error)
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

        void draw()
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
}