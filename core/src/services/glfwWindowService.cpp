#include <dory/core/registry.h>
#include <dory/core/services/glfwWindowSercvice.h>
#include <GLFW/glfw3.h>
#include <dory/core/resources/window.h>
#include <dory/core/repositories//iRepository.h>

namespace dory::core::services
{
    GlfwWindowService::GlfwWindowService(Registry &registry):
        _registry(registry)
    {}

    resources::IdType GlfwWindowService::createWindow(const resources::WindowParameters& parameters)
    {
        auto window = resources::entity::Window(resources::nullId, resources::WindowSystem::glfw);
        auto glfwWindowHandler = glfwCreateWindow((int)parameters.width, (int)parameters.height, parameters.title.c_str(), nullptr, nullptr);
        window.subsystemData = std::make_shared<resources::entity::GlfwWindow>(glfwWindowHandler);
        resources::IdType id;

        _registry.get<repositories::IRepository<resources::entity::Window>>([&window, &id](repositories::IRepository<resources::entity::Window>* repository) {
            id = repository->insert(window);
        });

        return id;
    }

    void GlfwWindowService::closeWindow(resources::IdType windowId)
    {
        _registry.get<repositories::IRepository<resources::entity::Window>>([&windowId](repositories::IRepository<resources::entity::Window>* repository) {
            auto window = repository->get(windowId);
            if(window && window->windowSystemType == resources::WindowSystem::glfw)
            {
                auto specificData = std::static_pointer_cast<resources::entity::GlfwWindow>(window->subsystemData);
                glfwDestroyWindow(specificData->handler);

                repository->remove(windowId);
            }
        });
    }
}