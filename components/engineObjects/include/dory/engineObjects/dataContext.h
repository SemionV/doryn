#pragma once

#include "dory/engine/resources/dataContext.h"

namespace dory
{
    struct ProjectDataContext
    {
    };

    extern template class dory::engine::resources::DataContext<ProjectDataContext>;
    using DataContextType = dory::engine::resources::DataContext<ProjectDataContext>;
}