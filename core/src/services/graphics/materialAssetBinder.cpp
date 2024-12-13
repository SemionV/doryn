#include <dory/core/registry.h>
#include <dory/core/services/graphics/materialAssetBinder.h>
#include "spdlog/fmt/fmt.h"

namespace dory::core::services::graphics
{
    using namespace repositories::assets;
    using namespace repositories::bindings;
    using namespace resources;
    using namespace resources::bindings;
    using namespace resources::entities;
    using namespace devices;

    MaterialAssetBinder::MaterialAssetBinder(Registry& registry): DependencyResolver(registry)
    {}

    void MaterialAssetBinder::bind(resources::IdType materialId, resources::entities::GraphicalContext& graphicalContext)
    {
        assert(!graphicalContext.materialBindings.contains(materialId)); //binding the previously bound asset

        auto materialRepo = _registry.get<IMaterialRepository>();
        auto materialBindingRepo = _registry.get<IMaterialBindingRepository>(graphicalContext.graphicalSystem);
        auto shaderBindingRepo = _registry.get<IShaderBindingRepository>(graphicalContext.graphicalSystem);
        auto gpuDevice = _registry.get<IGpuDevice>(graphicalContext.graphicalSystem);

        if(materialRepo && materialBindingRepo && gpuDevice)
        {
            auto material = materialRepo->get(materialId);
            auto materialBinding = materialBindingRepo->insert(MaterialBinding{});

            if(material && materialBinding)
            {
                materialBinding->material = *material;

                std::vector<ShaderBinding*> shaderBindings{};

                for(const auto shaderId : material->shaders)
                {
                    auto* shaderBinding = shaderBindingRepo->get(shaderId);
                    if(shaderBinding)
                    {
                        shaderBindings.emplace_back(shaderBinding);
                    }
                }

                gpuDevice->bindMaterial(materialBinding, shaderBindings);
                if(!materialBinding->linkingError.empty())
                {
                    _registry.get<core::services::ILogService>([&material, &materialBinding](core::services::ILogService* logger) {
                        logger->error(fmt::format("Shader Program linking error({0}):\n{1}",
                                                  material->id,
                                                  materialBinding->linkingError));
                    });
                }

                graphicalContext.materialBindings[materialId] = materialBinding->id;
            }
        }
    }
}