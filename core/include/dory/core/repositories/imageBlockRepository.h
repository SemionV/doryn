#pragma once

#include <dory/core/dependencyResolver.h>
#include <dory/core/repositories/iBlockRepository.h>

namespace dory::core::repositories
{
    class ImageBlockRepository: public DependencyResolver, public IBlockRepository
    {
    public:
        explicit ImageBlockRepository(Registry& registry);

        void storeBlock(const resources::entities::Block& block, const resources::entities::BlockStream& stream) final;
    };
}
