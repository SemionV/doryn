#include <dory/core/registry.h>
#include <dory/core/repositories/imageBlockRepository.h>

namespace dory::core::repositories
{
    using namespace resources::entities;

    ImageBlockRepository::ImageBlockRepository(Registry& registry): DependencyResolver(registry)
    {}

    void ImageBlockRepository::storeBlock(const Block& block, const BlockStream& stream)
    {

    }
}
