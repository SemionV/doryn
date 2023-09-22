#include "dependencies.h"
#include "project.h"

namespace testApp
{
    void Project::configureProject()
    {
        configuration = std::make_shared<configuration::FileSystemBasedConfiguration>("configuration");
        
        windowIdFactory = std::make_shared<NumberIdFactory<EntityId>>();
        windowRespository = std::make_shared<EntityRepository<openGL::GlfwWindow>>();

        pipelineService = std::make_shared<dory::domain::services::PipelineService>(pipelineNodeRepositoryReader);
    }

    void Project::configurePipeline()
    {
        
    }
}