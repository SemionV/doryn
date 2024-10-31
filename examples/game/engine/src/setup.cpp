#include "dory/game/setup.h"
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
#include <dory/core/services/serializer.h>
#include <dory/core/services/dataFormatResolver.h>
#include <dory/core/services/scriptService.h>
#include <dory/core/services/configurationService.h>
#include <dory/core/services/localizationService.h>
#include <dory/core/services/pipelineService.h>
#include <dory/core/services/frameService.h>

namespace dory::game
{
    void Setup::setupRegistry(const generic::extension::LibraryHandle& libraryHandle, core::Registry& registry)
    {
        registerEventBundle<core::events::pipeline::Bundle>(libraryHandle, registry);
        registerEventBundle<core::events::application::Bundle>(libraryHandle, registry);
        registerEventBufferBundle<core::events::io::Bundle>(libraryHandle, registry);
        registerEventBundle<core::events::script::Bundle>(libraryHandle, registry);
        registerEventBundle<core::events::window::Bundle>(libraryHandle, registry);

        registry.set<core::devices::IStandardIODevice>(libraryHandle, std::make_shared<core::devices::StandardIODevice>(registry));
        registry.set<core::devices::ITerminalDevice>(libraryHandle, std::make_shared<core::devices::TerminalDevice>(registry));

        registerRepository<core::resources::entity::Camera>(libraryHandle, registry);
        registerRepository<core::resources::entity::View>(libraryHandle, registry);
        registerRepository<core::resources::entity::Window>(libraryHandle, registry);
        registry.set<core::repositories::IPipelineRepository>(libraryHandle, std::make_shared<core::repositories::PipelineRepository>());

        registry.set<core::services::IFileService>(libraryHandle, std::make_shared<core::services::FileService>());
        registry.set<core::services::ILibraryService>(libraryHandle, std::make_shared<core::services::LibraryService>());

        std::shared_ptr<core::services::IMultiSinkLogService> appLogger = std::make_shared<core::services::LogService>();
        registry.set<core::services::IMultiSinkLogService, core::resources::Logger::App>(libraryHandle, appLogger);
        registry.set<core::services::ILogService, core::resources::Logger::App>(libraryHandle, appLogger);

        std::shared_ptr<core::services::IMultiSinkLogService> configLogger = std::make_shared<core::services::LogService>();
        registry.set<core::services::IMultiSinkLogService, core::resources::Logger::Config>(libraryHandle, configLogger);
        registry.set<core::services::ILogService, core::resources::Logger::Config>(libraryHandle, configLogger);

        registry.set<core::services::serialization::ISerializer, core::resources::DataFormat::yaml>(libraryHandle, std::make_shared<core::services::serialization::YamlSerializer>());
        registry.set<core::services::serialization::ISerializer, core::resources::DataFormat::json>(libraryHandle, std::make_shared<core::services::serialization::JsonSerializer>());
        registry.set<core::services::IDataFormatResolver>(libraryHandle, std::make_shared<core::services::DataFormatResolver>());

        registry.set<core::services::IScriptService>(libraryHandle, std::make_shared<core::services::ScriptService>(registry));
        registry.set<core::services::IConfigurationService>(libraryHandle, std::make_shared<core::services::ConfigurationService>(registry));
        registry.set<core::services::ILocalizationService>(libraryHandle, std::make_shared<core::services::LocalizationService>(registry));
        registry.set<core::services::IPipelineService>(libraryHandle, std::make_shared<core::services::PipelineService>(registry));
        registry.set<core::services::IFrameService>(libraryHandle, std::make_shared<core::services::FrameService>(registry));
    }
}