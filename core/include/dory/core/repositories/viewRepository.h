#pragma once

#include <dory/core/repositories/iViewRepository.h>
#include <dory/core/repository.h>

namespace dory::core::repositories
{
    class ViewRepository: public Repository<resources::entities::View, resources::IdType, IViewRepository>
    {
    public:
        std::vector<resources::IdType> getWindowViews(resources::IdType windowId) override;
    };
}
