#pragma once

#include "iRepository.h"
#include <dory/core/resources/entity.h>

namespace dory::core::repositories
{
    class IFileWatchRepository: public IRepository<resources::entity::FileWatch>
    {};
}
