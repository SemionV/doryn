#pragma once

#include "base/dependencies.h"
#include "engine/entity.h"

namespace dory::domain::entity
{
    using IdType = dory::entity::IdType;

    struct Camera: public dory::entity::Entity<dory::entity::IdType>
    {
        Camera(IdType id):
            Entity(id)
        {}
    };

    struct Window: public dory::entity::Entity<IdType>
    {
        Window(IdType id):
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

    struct View: public dory::entity::Entity<IdType>
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

    struct PipelineNode: dory::entity::Entity<IdType>
    {
        std::shared_ptr<void> attachedController;
        IdType parentNodeId;
        std::string name;
        int priority;

        PipelineNode(IdType id, std::shared_ptr<void> attachedController = nullptr, int priority = 0, IdType parentNodeId = dory::entity::nullId, std::string name = ""):
            Entity(id),
            attachedController(attachedController),
            parentNodeId(parentNodeId),
            priority(priority),
            name(name)
        {}
    };
}