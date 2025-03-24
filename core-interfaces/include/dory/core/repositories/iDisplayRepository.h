#pragma once

#include <dory/core/repositories/iRepository.h>
#include <dory/core/resources/entities/display.h>

namespace dory::core::repositories
{
    class IDisplayRepository: public IRepository<resources::entities::Display>
    {};
}