#pragma once

#include <dory/core/services/iGraphicalSystem.h>

namespace dory::core
{
    class Registry;
}

namespace dory::core::services
{
    class GraphicalSystem final : public IGraphicalSystem
    {
    private:
        core::Registry& _registry;

        void bindMeshData(resources::entities::GraphicalContext& graphicalContext, const resources::assets::Mesh* mesh, resources::bindings::MeshBinding* meshBinding);

    public:
        using VertexPositionDimensionType = float;

        explicit GraphicalSystem(core::Registry& registry);

        bool uploadProgram(core::resources::entities::ShaderProgram& program, const core::resources::entities::Window& window) override;
        void render(core::resources::DataContext& context, const core::resources::entities::Window& window) override;
        void bindMesh(const resources::IdType meshId, resources::entities::GraphicalContext& graphicalContext) override;
    };
}