#pragma once

#include <dory/core/repositories/iLayoutRepository.h>

namespace dory::core::repositories
{
    class LayoutRepository final : public Repository<resources::entities::layout::Layout, resources::IdType, ILayoutRepository>
    {};
}