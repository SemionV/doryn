#include <dory/core/registry.h>
#include <dory/core/services/glfwWindowService.h>
#include <GLFW/glfw3.h>
#include <dory/core/resources/glfwWindow.h>
#include <dory/core/repositories/iWindowRepository.h>
#include <dory/core/services/iGraphicalSystem.h>

namespace dory::core::services
{
    GlfwWindowService::GlfwWindowService(Registry &registry):
        WindowService(registry)
    {}

    resources::IdType GlfwWindowService::createWindow(const resources::WindowParameters& parameters)
    {
        auto window = resources::entities::GlfwWindow(resources::nullId, resources::WindowSystem::glfw, parameters.graphicalSystem);
        auto glfwWindowHandler = glfwCreateWindow((int)parameters.width, (int)parameters.height, parameters.title.c_str(), nullptr, nullptr);
        window.handler = glfwWindowHandler;
        return initializeWindow(window);
    }

    void GlfwWindowService::closeWindow(resources::IdType windowId)
    {
        _registry.get<repositories::IWindowRepository, resources::WindowSystem::glfw>([&windowId](repositories::IWindowRepository* repository) {
            auto window = repository->get(windowId);
            if(window)
            {
                auto* glfwWindow = (resources::entities::GlfwWindow*)window;
                glfwDestroyWindow(glfwWindow->handler);
            }
        });

        removeWindow(windowId);
    }
}