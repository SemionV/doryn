#pragma once

#include "iRepository.h"
#include <dory/core/resources/entities/window.h>

namespace dory::core::repositories
{
    class IWindowRepository: public IRepository<resources::entities::Window>
    {};
}
