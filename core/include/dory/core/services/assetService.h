#pragma once

#include <dory/core/services/iAssetService.h>

namespace dory::core
{
    class Registry;
}

namespace dory::core::services
{
    class AssetService: public IAssetService
    {
    private:
        Registry& _registry;

    public:
        explicit AssetService(Registry& registry);

        resources::IdType loadProgram(const resources::objects::ShaderProgram& program, const resources::entities::Window& window) override;
    };
}
