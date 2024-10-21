#include "dory/game/engine/setup.h"
#include <dory/core/services/fileService.h>
#include <dory/core/services/libraryService.h>

#ifdef DORY_PLATFORM_LINUX
#include <dory/core/devices/standardIoDeviceUnix.h>
#endif
#ifdef DORY_PLATFORM_WIN32
#include <dory/core/devices/standardIoDeviceWin32.h>
#endif

#include <dory/core/devices/terminalDevice.h>
#include <dory/core/generic/repository.h>
#include <dory/core/repositories/iRepository.h>
#include <dory/core/repositories/pipelineRepository.h>

namespace dory::game::engine
{
    void dory::game::engine::Setup::setupRegistry(const core::extensionPlatform::LibraryHandle& libraryHandle, core::Registry& registry)
    {
        registerEventBundle<core::events::mainController::Bundle>(libraryHandle, registry);
        registerEventBundle<core::events::application::Bundle>(libraryHandle, registry);
        registerEventBufferBundle<core::events::io::Bundle>(libraryHandle, registry);
        registerEventBundle<core::events::script::Bundle>(libraryHandle, registry);
        registerEventBundle<core::events::window::Bundle>(libraryHandle, registry);

        registry.set<core::devices::IStandardIODevice>(libraryHandle, std::make_shared<core::devices::StandardIODevice>(registry));
        registry.set<core::devices::ITerminalDevice>(libraryHandle, std::make_shared<core::devices::TerminalDevice>(registry));

        registerRepository<core::resources::entity::Camera>(libraryHandle, registry);
        registerRepository<core::resources::entity::View>(libraryHandle, registry);
        registerRepository<core::resources::entity::Window>(libraryHandle, registry);
        registry.set<core::repositories::IPipelineRepository, core::repositories::IPipelineNodeRepository>(libraryHandle, std::make_shared<core::repositories::PipelineRepository>());

        registry.set<core::services::IFileService>(libraryHandle, std::make_shared<core::services::FileService>());
        registry.set<core::services::ILibraryService>(libraryHandle, std::make_shared<core::services::LibraryService>());
    }
}