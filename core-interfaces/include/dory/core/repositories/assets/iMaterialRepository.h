#pragma once

#include "dory/core/resources/assets/material.h"

namespace dory::core::repositories::assets
{
    class IMaterialRepository: public IRepository<resources::assets::Material>
    {};
}
