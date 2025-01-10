#include <dory/core/registry.h>
#include <dory/core/repositories/imageBlockRepository.h>
#include <dory/core/resources/assetType.h>

namespace dory::core::repositories
{
    using namespace resources::entities;

    ImageBlockRepository::ImageBlockRepository(Registry& registry, const resources::configuration::Configuration& configuration):
        DependencyResolver(registry),
        _configuration(configuration)
    {}

    void ImageBlockRepository::storeBlock(const Block& block, const BlockStream& stream)
    {
        assert(block.blockType == BlockType::image);
        assert(stream.blockType == BlockType::image);

        if(auto imageFileService = _registry.get<services::files::IImageFileService>(resources::AssetFileFormat::bmp))
        {


            //TODO: use coroutines here
            imageFileService->save("", block);
        }
    }
}
