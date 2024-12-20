#pragma once

#include "dory/core/resources/scene/scene.h"
#include "iRepository.h"

namespace dory::core::repositories
{
    class ISceneRepository: public IRepository<resources::scene::Scene>
    {
    };
}