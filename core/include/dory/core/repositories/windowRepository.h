#pragma once

#include <dory/core/repositories/iWindowRepository.h>
#include <dory/core/repository.h>
#include <dory/core/resources/glfwWindow.h>

namespace dory::core::repositories
{
    class WindowRepository: public Repository<resources::entities::GlfwWindow, resources::IdType, IWindowRepository>
    {};
}

