#pragma once

#include "iRepository.h"
#include <dory/core/resources/entity.h>
#include <dory/core/resources/entities/view.h>
#include <vector>

namespace dory::core::repositories
{
    class IViewRepository: public IRepository<resources::entities::View>
    {
    public:
        virtual std::vector<resources::IdType> getWindowViews(resources::IdType windowId) = 0;
    };
}
