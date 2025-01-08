#pragma once

#include <dory/core/repositories/iBlockStreamRepository.h>
#include <dory/core/resources/blockStreamQueue.h>

namespace dory::core::repositories
{
    class BlockStreamRepository final : public Repository<resources::entities::BlockStreamQueue, resources::IdType, IBlockStreamRepository>
    {};
}
