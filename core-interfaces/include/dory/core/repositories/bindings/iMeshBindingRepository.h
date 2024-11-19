#pragma once

#include "dory/core/resources/bindings//meshBinding.h"

namespace dory::core::repositories::bindings
{
    class IMeshBindingRepository: public IRepository<resources::bindings::MeshBinding>
    {};
}
