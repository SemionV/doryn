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

    void dory::game::test_extension::Extension::attach(dory::core::extensionPlatform::LibraryHandle library, dory::core::resources::DataContext& dataContext)
    {
        std::cout << "dory::game::test_extension::Extension: Attach extension\n";

        _registry.set<core::services::IFileService>(library, _fileService.get());

        auto dispatcher = _registry.get<dory::core::events::script::IEventDispatcher>();
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
        std::cout << "dory::game::test_extension::Extension: Detach extension\n";
    }
}