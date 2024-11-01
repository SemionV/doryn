#include <dory/core/registry.h>
#include <dory/core/services/glfwWindowService.h>
#include <GLFW/glfw3.h>
#include <dory/core/resources/window.h>
#include <dory/core/repositories//iRepository.h>
#include <dory/core/services/iGraphicalSystem.h>

namespace dory::core::services
{
    GlfwWindowService::GlfwWindowService(Registry &registry):
        _registry(registry)
    {}

    resources::IdType GlfwWindowService::createWindow(const resources::WindowParameters& parameters)
    {
        auto window = resources::entity::Window(resources::nullId, resources::WindowSystem::glfw, parameters.graphicalSystem);
        auto glfwWindowHandler = glfwCreateWindow((int)parameters.width, (int)parameters.height, parameters.title.c_str(), nullptr, nullptr);
        window.windowSystemData = std::make_shared<resources::entity::GlfwWindow>(glfwWindowHandler);
        resources::IdType id;

        _registry.get<repositories::IRepository<resources::entity::Window>>([&window, &id](repositories::IRepository<resources::entity::Window>* repository) {
            id = repository->insert(window);
        });

        _registry.get<IGraphicalSystem>(parameters.graphicalSystem, [&window](IGraphicalSystem* graphicalSystem) {
            graphicalSystem->initializeGraphics(window);
        });

        return id;
    }

    void GlfwWindowService::closeWindow(resources::IdType windowId)
    {
        _registry.get<repositories::IRepository<resources::entity::Window>>([&windowId](repositories::IRepository<resources::entity::Window>* repository) {
            auto window = repository->get(windowId);
            if(window && window->windowSystem == resources::WindowSystem::glfw)
            {
                auto specificData = std::static_pointer_cast<resources::entity::GlfwWindow>(window->windowSystemData);
                glfwDestroyWindow(specificData->handler);

                repository->remove(windowId);
            }
        });
    }
}