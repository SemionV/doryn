#pragma once

#include <dory/generic/baseTypes.h>
#include <dory/core/resources/scene/scene.h>

namespace dory::core::services
{
    class ISceneBuilder: public generic::Interface
    {
    public:
        virtual resources::scene::Scene* build(const resources::scene::configuration::SceneConfiguration& configuration, resources::DataContext& context) = 0;
        virtual void destroy(resources::scene::Scene& scene, resources::DataContext& context) = 0;
    };
}
