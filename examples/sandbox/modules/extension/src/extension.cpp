#include <extension.h>

namespace dory::sandbox
{
    void Extension::attach(LibraryHandle library, ExtensionContext& moduleContext) {
        std::cout << "Attach extension\n";
        /*auto& registry = moduleContext.registry;
        registry.services.scriptService.addScript("ping-extension", [&registry](Registry::DataContextType& context, const std::map<std::string, std::any>& arguments)
        {
            registry.devices.terminalDevice.writeLine(fmt::format("-\u001B[31m{0}\u001B[0m-", "Hi :-) "));
            //registry.events.applicationDispatcher.fire(context, events::application::Exit{});
        });*/
    }

    Extension::~Extension() {
        std::cout << "Detach extension\n";
    }
}