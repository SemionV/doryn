#pragma once

#include <dory/core/services/graphics/iGraphicalContextBindingsService.h>

namespace dory::renderer::opengl
{
    class GraphicalContextBindingsService: public core::services::graphics::IGraphicalContextBindingsService
    {
    public:
        void bindMesh(core::resources::assets::Mesh* mesh) override;
        core::resources::bindings::MeshBinding* getMeshBinding() override;
    };
}
