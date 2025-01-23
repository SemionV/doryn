#pragma once

#include <dory/generic/baseTypes.h>
#include <dory/core/resources/dataContext.h>
#include <dory/core/resources/scene/scene.h>
#include <dory/core/resources/scene/sceneDto.h>

namespace dory::core::services
{
    class ISceneBuilder: public generic::Interface
    {
    public:
        virtual resources::scene::Scene* build(resources::DataContext& context) = 0;
    };
}
