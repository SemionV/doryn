#pragma once

#include <dory/generic/baseTypes.h>
#include <dory/core/resources/objects/shaderProgram.h>
#include <dory/core/resources/entities/window.h>
#include <dory/core/resources/assets/mesh.h>

namespace dory::core::services
{
    class IAssetService: public generic::Interface
    {
    public:
        virtual resources::IdType loadProgram(const resources::objects::ShaderProgram& program, const resources::entities::Window& window) = 0;
        virtual resources::assets::Mesh* getMesh(resources::IdType meshId) = 0;
    };
}
