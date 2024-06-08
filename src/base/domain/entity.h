#pragma once

#include "base/dependencies.h"
#include "types.h"

namespace dory::domain::entity
{
    using IdType = unsigned int;
    constexpr static const IdType nullId = 0;

    template<typename T = IdType>
    struct Entity
    {
        T id;
        explicit Entity(T id):
                id(id)
        {}
    };

    struct Camera: public Entity<IdType>
    {
        explicit Camera(IdType id):
            Entity(id)
        {}
    };

    struct Window: public Entity<IdType>
    {
        explicit Window(IdType id):
            Entity(id)
        {}
    };

    struct Viewport
    {
        int x;
        int y;
        int width;
        int height;

        Viewport(int x, int y, int width, int height):
            x(x),
            y(y),
            width(width),
            height(height)
        {            
        }
    };

    struct View: public Entity<IdType>
    {
        IdType windowId;
        IdType controllerNodeId;
        IdType cameraId;
        Viewport viewport;

        View(IdType id, IdType windowId, IdType controllerNodeId, IdType cameraId, Viewport viewport):
            Entity(id),
            windowId(windowId),
            controllerNodeId(controllerNodeId),
            cameraId(cameraId),
            viewport(viewport)
        {}
    };

    enum class PipelineNodePriority
    {
        Default = 0,
        First = 1
    };

    template<typename TDataContext>
    struct PipelineNode: Entity<IdType>
    {
        std::shared_ptr<void> attachedController;
        std::function<void(IdType referenceId, const TimeSpan& timeStep, TDataContext& context)> update;
        IdType parentNodeId;
        std::string name;
        PipelineNodePriority priority;

        explicit PipelineNode(IdType id,
                              std::shared_ptr<void> attachedController = nullptr,
                              PipelineNodePriority priority = PipelineNodePriority::Default,
                              IdType parentNodeId = nullId,
                              std::string name = ""):
            Entity(id),
            attachedController(std::move(attachedController)),
            parentNodeId(parentNodeId),
            priority(priority),
            name(std::move(name))
        {}

        template<typename F>
        explicit PipelineNode(F&& update,
                              PipelineNodePriority priority = PipelineNodePriority::Default,
                              IdType parentNodeId = nullId,
                              std::string name = ""):
                Entity(nullId),
                update(std::forward<F>(update)),
                attachedController(nullptr),
                parentNodeId(parentNodeId),
                priority(priority),
                name(std::move(name))
        {}
    };
}