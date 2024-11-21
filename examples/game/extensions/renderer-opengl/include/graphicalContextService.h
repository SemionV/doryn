#pragma once

#include <dory/core/services/graphics/iGraphicalContextService.h>

namespace dory::core
{
    class Registry;
}

namespace dory::renderer::opengl
{
    class GraphicalContextService: public core::services::graphics::IGraphicalContextService
    {
    private:
        core::Registry& _registry;

        void bindMesh(core::resources::entities::GraphicalContext* graphicalContext, core::resources::assets::Mesh* mesh);

    public:
        explicit GraphicalContextService(core::Registry& registry);

        void bindMesh(core::resources::assets::Mesh* mesh, core::resources::IdType sceneId) override;
        core::resources::bindings::MeshBinding* getMeshBinding(core::resources::IdType meshId, core::resources::IdType graphicalContextId) override;
    };
}
