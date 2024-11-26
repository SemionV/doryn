#pragma once

#include "dory/core/resources/bindings/materialBinding.h"

namespace dory::core::repositories::bindings
{
    class IMaterialBindingRepository: public IRepository<resources::bindings::MaterialBinding>
    {};
}
