#pragma once

#include <dory/core/repositories/iRepository.h>
#include <dory/core/resources/entities/layout.h>

namespace dory::core::repositories
{
    class ILayoutRepository: public IRepository<resources::entities::layout::Layout>
    {};
}