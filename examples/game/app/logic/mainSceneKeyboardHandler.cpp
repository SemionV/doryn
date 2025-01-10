#include <dory/core/registry.h>
#include "mainSceneKeyboardHandler.h"
#include <dory/core/resources/dataContext.h>

namespace dory::game::logic
{
    using namespace core;
    using namespace generic;
    using namespace generic::extension;

    MainSceneKeyboardHandler::MainSceneKeyboardHandler(Registry& registry, CameraService& cameraService):
        DependencyResolver(registry),
        _cameraService(cameraService)
    {}

    bool MainSceneKeyboardHandler::initialize(const LibraryHandle& libraryHandle, core::resources::DataContext& context)
    {
        auto windowListener = _registry.get<events::window::Bundle::IListener>();
        if(windowListener)
        {
            windowListener->attach([this](auto& context, const core::events::window::KeyboardEvent& event) {
                if(context.mainWindowId == event.windowId)
                {
                    auto* view = getWindowView(event.windowId, event.windowSystem);

                    if(view)
                    {
                        if(event.action == core::events::KeyAction::Press || event.action == core::events::KeyAction::Repeat)
                        {
                            if(event.keyCode == core::events::KeyCode::A || event.keyCode == core::events::KeyCode::Left)
                            {
                                _cameraService.moveCamera(MoveDirection::left, *view);
                            }
                            else if(event.keyCode == core::events::KeyCode::D || event.keyCode == core::events::KeyCode::Right)
                            {
                                _cameraService.moveCamera(MoveDirection::right, *view);
                            }
                            else if(event.keyCode == core::events::KeyCode::W || event.keyCode == core::events::KeyCode::Up)
                            {
                                _cameraService.moveCamera(MoveDirection::up, *view);
                            }
                            else if(event.keyCode == core::events::KeyCode::S || event.keyCode == core::events::KeyCode::Down)
                            {
                                _cameraService.moveCamera(MoveDirection::down, *view);
                            }
                        }
                        else if(event.action == core::events::KeyAction::Release)
                        {
                            _cameraService.stopCamera(*view);
                        }
                    }
                }
            });
        }

        return true;
    }

    core::resources::entities::View* MainSceneKeyboardHandler::getWindowView(core::resources::IdType windowId, core::resources::WindowSystem windowSystem)
    {
        auto windowRepository = _registry.get<core::repositories::IWindowRepository>();
        auto viewRepository = _registry.get<core::repositories::IViewRepository>();
        if(windowRepository && viewRepository)
        {
            auto window = windowRepository->get(windowId);
            if(window)
            {
                if(window->views.size() == 1)
                {
                    auto view = viewRepository->get(window->views[0]);
                    if(view)
                    {
                        return view;
                    }
                }
            }
        }

        return nullptr;
    }
}