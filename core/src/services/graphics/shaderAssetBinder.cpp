#include <dory/core/registry.h>
#include <dory/core/services/graphics/shaderAssetBinder.h>
#include "spdlog/fmt/fmt.h"

namespace dory::core::services::graphics
{
    using namespace repositories::assets;
    using namespace repositories::bindings;
    using namespace resources;
    using namespace resources::bindings;
    using namespace resources::entities;
    using namespace devices;

    ShaderAssetBinder::ShaderAssetBinder(Registry& registry) : DependencyResolver(registry)
    {}

    void ShaderAssetBinder::bind(IdType shaderId, GraphicalContext& graphicalContext)
    {
        assert(!graphicalContext.shaderBindings.contains(shaderId)); //binding the previously bound asset

        auto shaderRepo = _registry.get<IShaderRepository>();
        auto shaderBindingRepo = _registry.get<IShaderBindingRepository>(graphicalContext.graphicalSystem);
        auto gpuDevice = _registry.get<IGpuDevice>(graphicalContext.graphicalSystem);

        if(shaderRepo && shaderBindingRepo && gpuDevice)
        {
            auto shader = shaderRepo->get(shaderId);
            auto shaderBinding = shaderBindingRepo->insert(ShaderBinding{});

            if(shader && shaderBinding)
            {
                gpuDevice->bindShader(shader->sourceCode, shader->type, shaderBinding);
                graphicalContext.shaderBindings[shaderId] = shaderBinding->id;

                if(!shaderBinding->compilationError.empty())
                {
                    _registry.get<core::services::ILogService>([&shader, &shaderBinding](core::services::ILogService* logger) {
                        logger->error(fmt::format("Shader compilation error({0}, {1}):\n{2}",
                                                  shader->id,
                                                  shader->filename.string(),
                                                  shaderBinding->compilationError));
                    });
                }
            }
        }
    }
}