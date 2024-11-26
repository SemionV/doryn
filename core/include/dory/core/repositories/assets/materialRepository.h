#pragma once

#include <dory/core/repositories/assets/iMaterialRepository.h>

namespace dory::core::repositories::assets
{
    class MaterialRepository: public Repository<resources::assets::Material, resources::IdType, IMaterialRepository>
    {};
}