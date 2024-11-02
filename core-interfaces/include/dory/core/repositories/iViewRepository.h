#pragma once

#include "iRepository.h"
#include <dory/core/resources/entity.h>
#include <vector>

namespace dory::core::repositories
{
    class IViewRepository: public IRepository<resources::entity::View>
    {
    public:
        virtual std::vector<resources::IdType> getWindowViews(resources::IdType windowId) = 0;
    };
}
