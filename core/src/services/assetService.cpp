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

        auto shaderProgramRepository = _registry.get<repositories::IShaderProgramRepository>(window.graphicalSystem);
        if(shaderProgramRepository)
        {
            auto* programEntity = shaderProgramRepository->get(program.key, window.id);
            if(!programEntity)
            {
                programEntity = shaderProgramRepository->insert(resources::entities::ShaderProgram{{}, window.graphicalSystem, program.key, window.id});
            }

            if(!programEntity)
            {
                if(logger)
                {
                    logger->error("Failed to store shader program in repository: " + program.key);
                }
                return resources::nullId;
            }

            auto shaderRepository = _registry.get<repositories::IShaderRepository>(window.graphicalSystem);
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

            auto graphicalSystem = _registry.get<IGraphicalSystem>();
            if(graphicalSystem)
            {
                if(!graphicalSystem->uploadProgram(*programEntity, window) && logger)
                {
                    logger->error("Failed to store initialize shader program: " + program.key);
                }
            }
        }

        return resources::nullId;
    }
}