#pragma once

#include <dory/core/repositories/bindings/iMeshBindingRepository.h>
#include "dory/core/resources/bindings/openglMeshBinding.h"

namespace dory::core::repositories::bindings
{
    class MeshBindingRepository: public Repository<resources::bindings::OpenglMeshBinding, resources::IdType, IMeshBindingRepository>
    {};
}