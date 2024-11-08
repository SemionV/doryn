#pragma once

#include <dory/generic/baseTypes.h>
#include <dory/core/resources/objects/shaderProgram.h>
#include <dory/core/resources/entities/window.h>

namespace dory::core::services
{
    class IAssetService: public generic::Interface
    {
    public:
        virtual bool loadProgram(const resources::objects::ShaderProgram& program, const resources::entities::Window& window) = 0;
    };
}
