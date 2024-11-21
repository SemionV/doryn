#pragma once

#include <dory/core/repositories/iGraphicalContextRepository.h>
#include <dory/core/resources/entities/graphicalContext.h>

namespace dory::core::repositories
{
    class GraphicalContextRepository: public Repository<resources::entities::GraphicalContext, resources::IdType, IGraphicalContextRepository>
    {};
}
