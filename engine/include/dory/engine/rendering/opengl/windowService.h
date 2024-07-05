#pragma once

#include <dory/engine/domain/services/windowService.h>
#include <dory/engine/domain/entityRepository.h>
#include <dory/engine/domain/entity.h>
#include "glfwWindow.h"
#include "glfwWindowParameters.h"

namespace dory::opengl
{
    template<typename TWindowRepository>
    struct WindowServiceDependencies
    {
        using WindowRepositoryType = TWindowRepository;
    };

    template<typename T>
    requires(is_instance_v<T, WindowServiceDependencies>)
    class WindowService: public domain::services::IWindowService<WindowService<T>>
    {
    private:
        using WindowRepositoryType = domain::IEntityRepository<typename T::WindowRepositoryType, GlfwWindow, domain::entity::IdType>;
        WindowRepositoryType& windowRepository;

    public:
        explicit WindowService(WindowRepositoryType& windowRepository):
                windowRepository(windowRepository)
        {}

        auto createWindowImpl()
        {
            GlfwWindowParameters glfwWindowParameters;
            auto glfwWindowHandler = glfwCreateWindow(800, 600, "dory - glfw window", nullptr, nullptr);
            auto window = windowRepository.store(GlfwWindow(domain::entity::nullId, glfwWindowHandler));

            return window;
        }

        void closeWindowImpl(domain::entity::IdType windowId)
        {
            auto window = windowRepository.get(windowId);

            windowRepository.remove(windowId);

            if(window)
            {
                glfwDestroyWindow(window->handler);
            }
        }
    };
}