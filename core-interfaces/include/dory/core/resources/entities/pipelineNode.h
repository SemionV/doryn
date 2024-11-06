#pragma once

#include "../entity.h"
#include <dory/generic/model.h>
#include <functional>
#include <dory/generic/extension/resourceHandle.h>
#include <string>

namespace dory::core::resources::entities
{
    struct PipelineNode: public Entity<IdType>
    {
        using UpdateFunctionType = std::function<void(IdType referenceId, const generic::model::TimeSpan& timeStep, DataContext& context)>;
        using ControllerPointerType = std::shared_ptr<void>;

        std::optional<generic::extension::ResourceHandle<ControllerPointerType>> attachedController {};
        std::optional<generic::extension::ResourceHandle<UpdateFunctionType>> updateFunction {};
        IdType parentNodeId {};
        std::string name {};

        PipelineNode() = default;

        explicit PipelineNode(IdType id,
                              IdType parentNodeId = nullId,
                              std::string name = ""):
                Entity(id),
                parentNodeId(parentNodeId),
                name(std::move(name))
        {}

        explicit PipelineNode(generic::extension::ResourceHandle<ControllerPointerType> attachedController,
                              IdType parentNodeId = nullId,
                              std::string name = ""):
                attachedController(std::move(attachedController)),
                parentNodeId(parentNodeId),
                name(std::move(name))
        {}

        explicit PipelineNode(generic::extension::ResourceHandle<UpdateFunctionType> updateFunction,
                              IdType parentNodeId = nullId,
                              std::string name = ""):
                updateFunction(std::move(updateFunction)),
                parentNodeId(parentNodeId),
                name(std::move(name))
        {}
    };
}