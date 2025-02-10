#pragma once

#include <dory/core/resources/scene/configuration.h>

namespace dory::core::repositories
{
    class ISceneConfigurationRepository: public IRepository<resources::scene::configuration::SceneConfiguration>
    {};
}

