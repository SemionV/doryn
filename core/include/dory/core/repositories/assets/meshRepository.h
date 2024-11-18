#pragma once

#include <dory/core/repositories/assets/iMeshRepository.h>

namespace dory::core::repositories::assets
{
    class MeshRepository: public Repository<resources::assets::Mesh, resources::IdType, IMeshRepository>
    {};
}

