#pragma once

#include "olgDependencies.h"
#include "glfwWindow.h"
#include "shaderService.h"

namespace doryOpenGL
{
    class ViewControllerOpenGL: public dory::ViewController
    {
        private:
            enum VAO_IDs { Triangles, NumVAOs };
            enum Buffer_IDs { ArrayBuffer, NumBuffers };
            enum Attrib_IDs { vPosition = 0 };

            GLuint  VAOs[NumVAOs];
            GLuint  Buffers[NumBuffers];
            enum {NumVertices = 6};

        public:
            ViewControllerOpenGL(std::shared_ptr<dory::IConfiguration> configuration, std::shared_ptr<dory::View> view);

            void initialize(dory::DataContext& context) override;
            void stop(dory::DataContext& context) override;
            void update(const dory::TimeSpan& timeStep, dory::DataContext& context) override;
    };
}