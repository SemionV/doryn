#pragma once

#include "base/base.h"
#include "graphics/program.h"
#include "graphics/blocks.h"
#include "shaderService.h"

namespace dory::openGL
{
    class Renderer
    {
        private:
            graphics::Program program;

            graphics::ColorsBufferInterface colorsUniform;

            enum VAO_IDs { Triangles, NumVAOs };
            enum Buffer_IDs { ArrayBuffer, UniformBuffer, NumBuffers };
            enum Attrib_IDs { vPosition = 0 };

            GLuint  VAOs[NumVAOs];
            GLuint  Buffers[NumBuffers];
            enum {NumVertices = 6};

        public:
            virtual ~Renderer() = default;

            void initialize(std::shared_ptr<configuration::IConfiguration> configuration);
            void draw();

        private:
            virtual graphics::Program loadProgram(std::shared_ptr<configuration::IConfiguration> configuration);

            void bindProgram(graphics::Program program);
    };
}