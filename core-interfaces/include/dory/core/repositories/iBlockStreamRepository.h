#pragma once

#include "iRepository.h"
#include <dory/core/resources/entities/stream.h>

namespace dory::core::repositories
{
    class IBlockStreamRepository: public IRepository<resources::entities::BlockStream>
    {};
}
