#pragma once

#include "base/base.h"
#include "graphics/program.h"
#include "graphics/blocks.h"
#include "graphics/vertexArray.h"

namespace dory::openGL
{
    struct TrianglesVertexArray: public graphics::VertexArray<1>
    {
        TrianglesVertexArray():
            graphics::VertexArray<1>({graphics::VertexAttribute(2, GL_FLOAT)})
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

    struct ColorsUniformBlock: public graphics::UniformBlock<3>
    {
        static constexpr std::string_view pointLiteral = ".";
        static constexpr std::string_view blockNameLiteral = "ColorsBlock";
        static constexpr std::string_view prefixLiteral = dory::compileTime::JoinStringLiterals<blockNameLiteral, pointLiteral>;

        static constexpr std::string_view brightColorLiteral = "brightColor";
        static constexpr std::string_view hippieColorLiteral = "hippieColor";
        static constexpr std::string_view darkColorLiteral = "darkColor";

        ColorsUniformBlock():
            graphics::UniformBlock<3>(blockNameLiteral, 
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

    class Renderer
    {
        private:
            TrianglesProgram trianglesProgram;
            TrianglesVertexArray trianglesVertexArray;
            
            ColorsBufferInterface colorsUniformData {
                domain::Color(0.7f, 0.7f, 0.7f, 1.0f), 
                domain::Color(0.7f, 0.7f, 0.0f, 1.0f), 
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
            virtual ~Renderer() = default;

            virtual void initialize(std::shared_ptr<configuration::IConfiguration> configuration);
            virtual void draw();
    };
}