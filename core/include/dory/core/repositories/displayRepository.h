#pragma once

#include <dory/core/repositories/iDisplayRepository.h>
#include <dory/core/resources/glfwDisplay.h>

namespace dory::core::repositories
{
    class DisplayRepository final : public Repository<resources::entities::GlfwDisplay, resources::IdType, IDisplayRepository>
    {};
}