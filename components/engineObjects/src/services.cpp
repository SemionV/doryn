#include <dory/engineObjects/services.h>

namespace dory::engine::services
{
    template class IFrameService<BasicFrameService>;
    template void IFrameService<BasicFrameService>::startLoop(DataContextType& context, controllers::IMainController<MainControllerType>& engine);

    template class IFileService<FileService>;
}