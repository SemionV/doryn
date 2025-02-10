#pragma once

#include <dory/core/repositories/iSceneConfigurationRepository.h>

namespace dory::core::repositories
{
    class SceneConfigurationRepository final : public Repository<resources::scene::configuration::SceneConfiguration, resources::IdType, ISceneConfigurationRepository>
    {};
}