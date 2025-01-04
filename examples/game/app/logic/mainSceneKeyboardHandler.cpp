#include <dory/core/registry.h>
#include "mainSceneKeyboardHandler.h"

namespace dory::game::logic
{
    using namespace core;
    using namespace generic;
    using namespace generic::extension;
    using namespace resources;

    MainSceneKeyboardHandler::MainSceneKeyboardHandler(Registry& registry) : DependencyResolver(registry)
    {}

    bool MainSceneKeyboardHandler::initialize(const LibraryHandle& libraryHandle, DataContext& context)
    {
        auto windowListener = _registry.get<events::window::Bundle::IListener>();
        if(windowListener)
        {
            windowListener->attach([this](auto& context, const core::events::window::KeyboardEvent& event){
                auto logger = _registry.get<services::ILogService>();

                if(context.mainWindowId == event.windowId)
                {
                    if((event.keyCode == core::events::KeyCode::A || event.keyCode == core::events::KeyCode::Left)
                       && (event.action == core::events::KeyAction::Press || event.action == core::events::KeyAction::Repeat))
                    {
                        if(logger)
                        {
                            logger->information(std::string_view{ "Key A is pressed" });
                        }
                    }
                }
            });
        }

        return true;
    }
}