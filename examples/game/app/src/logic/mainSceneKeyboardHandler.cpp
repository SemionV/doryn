#include <dory/core/registry.h>
#include <dory/game/logic/mainSceneKeyboardHandler.h>
#include <dory/core/resources/dataContext.h>

namespace dory::game::logic
{
    using namespace core;
    using namespace resources;
    using namespace generic;
    using namespace generic::extension;

    MainSceneKeyboardHandler::MainSceneKeyboardHandler(Registry& registry, CameraService& cameraService):
        DependencyResolver(registry),
        _cameraService(cameraService)
    {}

    bool MainSceneKeyboardHandler::initialize(const LibraryHandle& libraryHandle, DataContext& context)
    {
        auto windowListener = _registry.get<events::window::Bundle::IListener>();
        if(windowListener)
        {
            windowListener->attach([this](auto& context, const events::window::KeyboardEvent& event) {
                if(context.mainWindowId == event.windowId)
                {
                    auto* view = getWindowView(event.windowId, event.windowSystem);

                    if(view)
                    {
                        if(event.action == events::KeyAction::Press || event.action == events::KeyAction::Repeat)
                        {
                            if(event.keyCode == events::KeyCode::A || event.keyCode == events::KeyCode::Left)
                            {
                                _cameraService.moveCamera(context, MoveDirection::left, *view);
                            }
                            else if(event.keyCode == events::KeyCode::D || event.keyCode == events::KeyCode::Right)
                            {
                                _cameraService.moveCamera(context, MoveDirection::right, *view);
                            }
                            else if(event.keyCode == events::KeyCode::W || event.keyCode == events::KeyCode::Up)
                            {
                                _cameraService.moveCamera(context, MoveDirection::up, *view);
                            }
                            else if(event.keyCode == events::KeyCode::S || event.keyCode == events::KeyCode::Down)
                            {
                                _cameraService.moveCamera(context, MoveDirection::down, *view);
                            }
                        }
                        else if(event.action == events::KeyAction::Release)
                        {
                            _cameraService.stopCamera(context, *view);
                        }
                    }
                }
            });
        }

        return true;
    }

    entities::View* MainSceneKeyboardHandler::getWindowView(IdType windowId, WindowSystem windowSystem)
    {
        auto windowRepository = _registry.get<repositories::IWindowRepository>();
        auto viewRepository = _registry.get<repositories::IViewRepository>();
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