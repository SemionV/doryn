#pragma once

#include <dory/generic/baseTypes.h>
#include <dory/core/resources/entity.h>
#include <dory/core/resources/entities/window.h>
#include <dory/core/resources/entities/shaderProgram.h>
#include <dory/core/resources/dataContext.h>

namespace dory::core::services
{
    class IGraphicalSystem: generic::Interface
    {
    public:
        virtual bool uploadProgram(const resources::entities::ShaderProgram& program, const resources::entities::Window& window) = 0;
        virtual void render(resources::DataContext& context, const resources::entities::Window& window) = 0;
    };
}
