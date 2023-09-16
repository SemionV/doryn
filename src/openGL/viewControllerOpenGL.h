#pragma once

#include "glfwWindow.h"
#include "shaderService.h"

namespace dory::openGL
{
    class ViewControllerOpenGL: public dory::ViewController<GlfwWindow>
    {
        private:
            std::shared_ptr<EntityAccessor<GlfwWindow>> windowRespository;

            enum VAO_IDs { Triangles, NumVAOs };
            enum Buffer_IDs { ArrayBuffer, NumBuffers };
            enum Attrib_IDs { vPosition = 0 };

            GLuint  VAOs[NumVAOs];
            GLuint  Buffers[NumBuffers];
            enum {NumVertices = 6};

        public:
            ViewControllerOpenGL(int viewId, 
                std::shared_ptr<EntityAccessor<View>> viewRepository, 
                std::shared_ptr<IConfiguration> configuration,
                std::shared_ptr<EntityAccessor<GlfwWindow>> windowRespository);

            bool initialize(dory::DataContext& context) override;
            void stop(dory::DataContext& context) override;
            void update(const int referenceId, const dory::TimeSpan& timeStep, dory::DataContext& context) override;
    };
}