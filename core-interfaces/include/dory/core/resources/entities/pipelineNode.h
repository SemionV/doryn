#pragma once

#include "../entity.h"
#include <dory/generic/model.h>
#include <functional>
#include <dory/generic/extension/resourceHandle.h>
#include <string>
#include <optional>
#include <utility>

namespace dory::core::resources::entities
{
    struct NodeUpdateCounter
    {
        std::size_t count;
        generic::model::TimeSpan deltaTime;
    };

    struct PipelineNode: public Entity<>
    {
        using UpdateFunctionType = std::function<void(IdType nodeId, const generic::model::TimeSpan& timeStep, DataContext& context)>;
        using UpdateTriggerType = std::function<NodeUpdateCounter(IdType nodeId, const generic::model::TimeSpan& timeStep, DataContext& context)>;
        using ControllerPointerType = std::shared_ptr<void>;

        std::optional<generic::extension::ResourceHandle<ControllerPointerType>> attachedController {};
        std::optional<generic::extension::ResourceHandle<UpdateFunctionType>> updateFunction {};
        std::optional<generic::extension::ResourceHandle<UpdateTriggerType>> updateTrigger {};
        IdType parentNodeId {};
        std::string name {};
        bool skipUpdate {};

        PipelineNode() = default;

        explicit PipelineNode(const IdType id,
                              const IdType parentNodeId = nullId,
                              std::string name = ""):
                Entity(id),
                parentNodeId(parentNodeId),
                name(std::move(name))
        {}

        explicit PipelineNode(const IdType id,
                              generic::extension::ResourceHandle<UpdateTriggerType> updateTrigger,
                              const IdType parentNodeId = nullId,
                              std::string name = ""):
                Entity(id),
                updateTrigger(updateTrigger),
                parentNodeId(parentNodeId),
                name(std::move(name))
        {}

        explicit PipelineNode(generic::extension::ResourceHandle<ControllerPointerType> attachedController,
                              const IdType parentNodeId = nullId,
                              std::string name = ""):
                attachedController(std::move(attachedController)),
                parentNodeId(parentNodeId),
                name(std::move(name))
        {}

        explicit PipelineNode(generic::extension::ResourceHandle<ControllerPointerType> attachedController,
                              generic::extension::ResourceHandle<UpdateTriggerType> updateTrigger,
                              const IdType parentNodeId = nullId,
                              std::string name = ""):
                attachedController(std::move(attachedController)),
                updateTrigger(updateTrigger),
                parentNodeId(parentNodeId),
                name(std::move(name))
        {}

        explicit PipelineNode(generic::extension::ResourceHandle<UpdateFunctionType> updateFunction,
                              const IdType parentNodeId = nullId,
                              std::string name = ""):
                updateFunction(std::move(updateFunction)),
                parentNodeId(parentNodeId),
                name(std::move(name))
        {}

        explicit PipelineNode(generic::extension::ResourceHandle<UpdateFunctionType> updateFunction,
                              generic::extension::ResourceHandle<UpdateTriggerType> updateTrigger,
                              const IdType parentNodeId = nullId,
                              std::string name = ""):
                updateFunction(std::move(updateFunction)),
                updateTrigger(updateTrigger),
                parentNodeId(parentNodeId),
                name(std::move(name))
        {}
    };
}