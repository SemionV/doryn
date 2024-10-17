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

    std::string FileService::getMessage()
    {
        return "Hello from Extension!";
    }

    void dory::game::test_extension::Extension::attach(dory::core::extensionPlatform::LibraryHandle library, dory::core::resources::DataContext& dataContext, dory::core::Registry& registry)
    {
        _registry = &registry;
        _dataContext = &dataContext;

        std::cout << "dory::game::test_extension::Extension: Attach extension\n";

        auto fileService = std::make_shared<FileService>();
        _registry->services.setFileService(library, fileService);

        _registry->events.scriptDispatcher->fire(*_dataContext, core::events::script::Run{"test-script"});
    }

    Extension::~Extension()
    {
        std::cout << "dory::game::test_extension::Extension: Detach extension\n";
    }
}