#include <extension.h>
#include <iostream>
#include <dory/core/registry.h>

namespace dory::game::test_extension
{
    class FileService: public core::services::IFileService
    {
    public:
        std::string read(const std::filesystem::path& filePath) final
        {
            return "";
        }

        void write(const std::filesystem::path& filePath, const std::string& content) final
        {
        }

        std::string getMessage() final
        {
            return "Hello from Extension!";
        }
    };

    void dory::game::test_extension::Extension::attach(dory::core::extensionPlatform::LibraryHandle library, const dory::core::resources::ExtensionContext& extensionContext)
    {
        _registry = extensionContext.registry;

        std::cout << "dory::game::test_extension::Extension: Attach extension\n";

        auto fileService = std::make_shared<FileService>();
        extensionContext.registry->services.setFileService(library, fileService);

        auto dataContext = core::resources::DataContext{};
        extensionContext.registry->events.scriptDispatcher->fire(dataContext, core::events::script::Run{"test-script"});
    }

    Extension::~Extension()
    {
        std::cout << "dory::game::test_extension::Extension: Detach extension\n";
    }
}