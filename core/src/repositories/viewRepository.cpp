#include <dory/core/repositories/viewRepository.h>
#include <algorithm>
#include <iterator>

namespace dory::core::repositories
{

    std::vector<resources::IdType> ViewRepository::getWindowViews(resources::IdType windowId)
    {
        std::vector<resources::IdType> views {};

        for(const auto& view : this->container)
        {
            if(view.windowId == windowId)
            {
                views.emplace_back(view.id);
            }
        }

        return views;
    }
}