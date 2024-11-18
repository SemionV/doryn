#pragma once

#include <dory/core/repositories/iGraphicalContextBindingsRepository.h>

namespace dory::core::repositories
{
    class GraphicalContextBindingsRepository: public Repository<resources::assets::Mesh, resources::IdType, IGraphicalContextBindingsRepository>
    {};
}
