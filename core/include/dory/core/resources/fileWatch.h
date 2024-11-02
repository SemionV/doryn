#pragma once

#include <dory/core/resources/entity.h>
#include <efsw/efsw.hpp>

namespace dory::core::resources::entity
{
    class EntropiaFileWatch: public FileWatchSpecificData
    {
    public:
        efsw::WatchID watchId;
    };
}
