#include <dory/core/registry.h>
#include <dory/core/services/assetService.h>

namespace dory::core::services
{
    AssetService::AssetService(Registry &registry):
        _registry(registry)
    {}

    resources::IdType AssetService::loadProgram(const resources::objects::ShaderProgram& program,
                                   const resources::entities::Window& window)
    {
        auto logger = _registry.get<ILogService>();

        resources::GraphicalSystem graphicalSystem = resources::GraphicalSystem::opengl;
        auto graphicalContextRepo = _registry.get<repositories::IGraphicalContextRepository>();
        if(graphicalContextRepo)
        {
            auto graphicalContext = graphicalContextRepo->get(window.graphicalContextId);
            if(graphicalContext)
            {
                graphicalSystem = graphicalContext->graphicalSystem;
            }
        }

        auto shaderProgramRepository = _registry.get<repositories::IShaderProgramRepository>(graphicalSystem);
        if(shaderProgramRepository)
        {
            auto* programEntity = shaderProgramRepository->get(program.key, window.id);
            if(!programEntity)
            {
                programEntity = shaderProgramRepository->insert(resources::entities::ShaderProgram{{}, graphicalSystem, program.key, window.id});
            }

            if(!programEntity)
            {
                if(logger)
                {
                    logger->error("Failed to store shader program in repository: " + program.key);
                }
                return resources::nullId;
            }

            auto shaderRepository = _registry.get<repositories::IShaderRepository>(graphicalSystem);
            if(shaderRepository)
            {
                for(const auto& shader : program.shaders)
                {
                    if(!shaderRepository->insert(resources::entities::Shader{{}, shader.fileName, programEntity->id}) && logger)
                    {
                        logger->error("Failed to store shader in repository: " + program.key + ", " + shader.fileName);
                    }
                }
            }

            /*auto graphicalSystemService = _registry.get<IGraphicalSystem>();
            if(graphicalSystemService)
            {
                if(!graphicalSystemService->uploadProgram(*programEntity, window) && logger)
                {
                    logger->error("Failed to initialize shader program: " + program.key);
                }
            }*/
        }

        return resources::nullId;
    }

    resources::assets::Mesh* AssetService::getMesh(resources::IdType meshId)
    {
        auto meshRepository = _registry.get<repositories::assets::IMeshRepository>();
        if(meshRepository)
        {
            return meshRepository->get(meshId);
        }

        return nullptr;
    }
}
