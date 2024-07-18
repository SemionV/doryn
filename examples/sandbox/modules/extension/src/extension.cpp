#include <extension.h>

namespace dory::sandbox
{
    Extension::Extension(ExtensionContext &extensionContext):
        _extensionContext(extensionContext),
        _pingScriptFunction{ std::bind(&Extension::pingScript, this, std::placeholders::_1, std::placeholders::_2) }
    {}

    Extension::~Extension()
    {
        std::cout << "Detach extension\n";
    }

    void Extension::attach(LibraryHandle library)
    {
        std::cout << "Attach extension\n";

        auto& registry = _extensionContext.registry;
        registry.services.scriptService.addScript("ping-extension", library, &_pingScriptFunction);
    }

    void Extension::pingScript(Registry::DataContextType& context, const std::map<std::string, std::any>& arguments)
    {
        auto& registry = _extensionContext.registry;
        registry.devices.terminalDevice.writeLine(fmt::format("\u001B[31m{0}\u001B[0m", "Hi :-) "));
    }
}