#pragma once

#include <dory/generic/baseTypes.h>
#include <dory/core/resources/entities/stream.h>

namespace dory::core::repositories
{
    class IBlockRepository: public generic::Interface
    {
    public:
        virtual void storeBlock(const resources::entities::Block& block, const resources::entities::BlockStream& stream) = 0;
    };
}