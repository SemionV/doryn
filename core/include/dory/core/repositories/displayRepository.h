#pragma once

#include <dory/core/repositories/iDisplayRepository.h>

namespace dory::core::repositories
{
    class DisplayRepository final : public Repository<resources::entities::Display, resources::IdType, IDisplayRepository>
    {};
}