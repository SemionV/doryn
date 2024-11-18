#pragma once

#include "dory/core/resources/assets/mesh.h"

namespace dory::core::repositories::assets
{
    class IMeshRepository: public IRepository<resources::assets::Mesh>
    {};
}