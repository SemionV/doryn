#include <extension.h>
#include <iostream>

namespace dory::game::test_extension
{
    std::string FileService::read(const std::filesystem::path& filePath)
    {
        return "";
    }

    void FileService::write(const std::filesystem::path& filePath, const std::string& content)
    {
    }

    void dory::game::test_extension::Extension::attach(dory::generic::extension::LibraryHandle library, dory::core::resources::DataContext& dataContext)
    {
        _registry.get<core::services::ILogService, core::resources::Logger::App>([](core::services::ILogService* logger) {
            logger->information(std::string_view ("dory::game::test_extension::Extension: attach extension"));
        });

        //_registry.set<core::services::IFileService>(library, _fileService.get());

        auto dispatcher = _registry.get<dory::core::events::script::Bundle::IDispatcher>();
        if(dispatcher)
        {
            dispatcher->fire(dataContext, core::events::script::Run{"test-script"});
        }
    }

    Extension::Extension(core::Registry &registry):
        _registry(registry),
        _fileService(registry, std::make_shared<FileService>())
    {}

    Extension::~Extension()
    {
        _registry.get<core::services::ILogService, core::resources::Logger::App>([](core::services::ILogService* logger) {
            logger->information(std::string_view ("dory::game::test_extension::Extension: detach extension"));
        });
    }
}