#pragma once

#include "iRepository.h"
#include <dory/core/resources/entities/stream.h>

namespace dory::core::repositories
{
    class IImageStreamRepository: public IRepository<resources::entities::ImageStream>
    {};
}
