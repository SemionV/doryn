#pragma once

#include <dory/core/services/graphics/iGraphicalContextBindingsService.h>

namespace dory::core
{
    class Registry;
}

namespace dory::renderer::opengl
{
    class GraphicalContextBindingsService: public core::services::graphics::IGraphicalContextBindingsService
    {
    private:
        core::Registry& _registry;

        void bindMesh(core::resources::entities::Window* window, core::resources::entities::GraphicalContextBindings* graphicalContextBindings, core::resources::assets::Mesh* mesh);

    public:
        explicit GraphicalContextBindingsService(core::Registry& registry);

        void bindMesh(core::resources::assets::Mesh* mesh, core::resources::IdType sceneId) override;
        core::resources::bindings::MeshBinding* getMeshBinding(core::resources::IdType meshId, core::resources::IdType graphicalContextBindingsId) override;
    };
}
