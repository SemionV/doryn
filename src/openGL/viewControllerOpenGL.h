#pragma once

#include "glfwWindow.h"
#include "shaderService.h"

namespace dory::openGL
{
    class ViewControllerOpenGL: public dory::domain::ViewController<GlfwWindow>
    {
        private:
            std::shared_ptr<RepositoryReader<GlfwWindow>> windowRespository;

            enum VAO_IDs { Triangles, NumVAOs };
            enum Buffer_IDs { ArrayBuffer, NumBuffers };
            enum Attrib_IDs { vPosition = 0 };

            GLuint  VAOs[NumVAOs];
            GLuint  Buffers[NumBuffers];
            enum {NumVertices = 6};

        public:
            ViewControllerOpenGL(std::shared_ptr<RepositoryReader<domain::entity::View>> viewRepository, 
                std::shared_ptr<IConfiguration> configuration,
                std::shared_ptr<RepositoryReader<GlfwWindow>> windowRespository);

            bool initialize(domain::entity::IdType referenceId, dory::DataContext& context) override;
            void stop(domain::entity::IdType referenceId, dory::DataContext& context) override;
            void update(domain::entity::IdType referenceId, const dory::TimeSpan& timeStep, dory::DataContext& context) override;
    };
}