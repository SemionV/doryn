#pragma once

#include <dory/core/repositories/bindings/iMaterialBindingRepository.h>
#include "dory/core/resources/bindings/openglMaterialBinding.h"

namespace dory::core::repositories::bindings
{
    class MaterialBindingRepository: public Repository<resources::bindings::OpenglMaterialBinding, resources::IdType, IMaterialBindingRepository>
    {};
}