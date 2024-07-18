#include <extension.h>

namespace dory::sandbox
{
#ifdef __unix__
    __attribute__((constructor))
    void on_load() {
        std::cout << "Extension Library loaded" << std::endl;
    }

    __attribute__((destructor))
    void on_unload() {
        std::cout << "Extension Library unloaded" << std::endl;
    }
#endif

    Extension::Extension(ExtensionContext &extensionContext):
        _extensionContext(extensionContext)
    {}

    Extension::~Extension()
    {
        std::cout << "Detach extension\n";
        auto& registry = _extensionContext.registry;
        registry.services.scriptService.removeScript("ping-ext");
    }

    void Extension::attach(LibraryHandle library)
    {
        std::cout << "Attach extension\n";

        auto& registry = _extensionContext.registry;

        auto script = std::bind(&Extension::pingScript, this, std::placeholders::_1, std::placeholders::_2);
        registry.services.scriptService.addScript("ping-ext", library, script);
    }

    void Extension::pingScript(Registry::DataContextType& context, const std::map<std::string, std::any>& arguments)
    {
        auto& registry = _extensionContext.registry;
        registry.devices.terminalDevice.writeLine(fmt::format("\u001B[31m{0}\u001B[0m", "Hello from Sandbox Extension module! :-) "));
    }
}