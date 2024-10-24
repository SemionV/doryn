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
#include <dory/core/repositories/iRepository.h>
#include <dory/core/repositories/pipelineRepository.h>
#include <dory/core/services/logServiceNull.h>
#include <dory/core/services/logService.h>
#include <dory/core/services/serializationService.h>

namespace dory::game::engine
{
    void dory::game::engine::Setup::setupRegistry(const generic::extension::LibraryHandle& libraryHandle, core::Registry& registry)
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
        auto pipelineRepository = std::make_shared<core::repositories::PipelineRepository>();
        registry.set<core::repositories::IPipelineRepository>(libraryHandle, pipelineRepository);
        registry.set<core::repositories::IPipelineNodeRepository>(libraryHandle, pipelineRepository);

        registry.set<core::services::IFileService>(libraryHandle, std::make_shared<core::services::FileService>());
        registry.set<core::services::ILibraryService>(libraryHandle, std::make_shared<core::services::LibraryService>());

        std::shared_ptr<core::services::IMultiSinkLogService> appLogger = std::make_shared<core::services::LogService>();
        registry.set<core::services::IMultiSinkLogService, core::Logger::App>(libraryHandle, appLogger);
        registry.set<core::services::IMultiSinkLogService, core::Logger::Config>(libraryHandle, std::make_shared<core::services::LogService>());

        registry.set<core::services::serialization::ISerializer, core::DataFormat::Yaml>(libraryHandle, std::make_shared<core::services::serialization::YamlSerializer>());
    }
}