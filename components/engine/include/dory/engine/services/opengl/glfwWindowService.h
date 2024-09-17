#pragma once

#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <dory/engine/services/windowService.h>
#include "dory/engine/repositories/entityRepository.h"
#include "dory/engine/resources/entity.h"
#include <dory/engine/resources/opengl/glfwWindow.h>
#include <dory/engine/resources/opengl/glfwWindowParameters.h>

namespace dory::engine::services::opengl
{
    template<typename TWindowRepository>
    struct GlfwWindowServiceDependencies
    {
        using WindowRepositoryType = TWindowRepository;
    };

    template<typename T>
    requires(is_instance_v<T, GlfwWindowServiceDependencies>)
    class GlfwWindowService: public IWindowService<GlfwWindowService<T>>
    {
    private:
        using WindowRepositoryType = repositories::IEntityRepository<typename T::WindowRepositoryType, resources::opengl::GlfwWindow, resources::entity::IdType>;
        WindowRepositoryType& windowRepository;

    public:
        explicit GlfwWindowService(WindowRepositoryType& windowRepository):
                windowRepository(windowRepository)
        {}

        auto createWindowImpl()
        {
            resources::opengl::GlfwWindowParameters glfwWindowParameters;
            auto glfwWindowHandler = glfwCreateWindow(800, 600, "dory - glfw window", nullptr, nullptr);
            auto window = windowRepository.store(resources::opengl::GlfwWindow(resources::entity::nullId, glfwWindowHandler));

            return window;
        }

        void closeWindowImpl(resources::entity::IdType windowId)
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