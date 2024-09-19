#pragma once

#include "dataContext.h"
#include "dory/engine/repositories/entityRepository.h"
#include <dory/engine/repositories/pipelineRepository.h>
#include <dory/engine/resources/entity.h>
#include <dory/engine/resources/opengl/glfwWindow.h>

namespace dory::engine::repositories
{
    extern template class EntityRepository<resources::entity::Camera>;
    using CameraRepositoryType = EntityRepository<resources::entity::Camera>;

    extern template class EntityRepository<resources::entity::View>;
    using ViewRepositoryType = EntityRepository<resources::entity::View>;

    extern template class EntityRepository<resources::opengl::GlfwWindow>;
    using WindowRepositoryType = EntityRepository<resources::opengl::GlfwWindow>;

    extern template class PipelineRepository<DataContextType, resources::entity::PipelineNode<DataContextType>>;
    using PipelineRepositoryType = PipelineRepository<DataContextType, resources::entity::PipelineNode<DataContextType>>;

    struct RepositoryLayer
    {
        CameraRepositoryType cameras;
        ViewRepositoryType views;
        WindowRepositoryType windows;
        PipelineRepositoryType pipelines;
    };
}