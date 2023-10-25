#pragma once

#include "base/base.h"
#include "graphics/program.h"
#include "graphics/blocks.h"
#include "graphics/vertexArray.h"
#include "graphics/openglProcedures.h"
#include "shaderService.h"

namespace dory::openGL
{
    struct TrianglesVertexArray: public graphics::VertexArray<1>
    {
        std::array<GLfloat, 12> verticesData = {
            -0.90f, -0.90f, 
            0.85f, -0.90f,
            -0.90f,  0.85f,
            
            0.90f, -0.85f,
            0.90f,  0.90f,
            -0.85f,  0.90f
        };

        TrianglesVertexArray():
            graphics::VertexArray<1>({graphics::VertexAttribute(2, GL_FLOAT)})
        {}
    };

    struct ColorsBufferInterface
    {
       domain::Color brightColor;
       domain::Color hippieColor;
       domain::Color darkColor;
    };

    struct ColorsUniformBlock: public graphics::UniformBlock<3>
    {
        static constexpr std::string_view pointLiteral = ".";
        static constexpr std::string_view blockNameLiteral = "ColorsBlock";
        static constexpr std::string_view prefixLiteral = dory::compileTime::JoinStringLiterals<blockNameLiteral, pointLiteral>;

        static constexpr std::string_view brightColorLiteral = "brightColor";
        static constexpr std::string_view hippieColorLiteral = "hippieColor";
        static constexpr std::string_view darkColorLiteral = "darkColor";

        ColorsBufferInterface colors;

        ColorsUniformBlock():
            graphics::UniformBlock<3>(blockNameLiteral, 
                dory::compileTime::JoinStringLiterals<prefixLiteral, brightColorLiteral>, 
                dory::compileTime::JoinStringLiterals<prefixLiteral, hippieColorLiteral>, 
                dory::compileTime::JoinStringLiterals<prefixLiteral, darkColorLiteral>)
        {}
    };

    class Renderer
    {
        private:
            graphics::Program program;
            TrianglesVertexArray trianglesVertexArray;
            ColorsUniformBlock colorsUniformBlock;

        public:
            virtual ~Renderer() = default;

            void initialize(std::shared_ptr<configuration::IConfiguration> configuration);
            void draw();

        private:
            virtual graphics::Program loadProgram(std::shared_ptr<configuration::IConfiguration> configuration);
    };
}