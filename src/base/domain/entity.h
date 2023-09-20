#pragma once

#include "base/dependencies.h"

namespace dory::domain::entity
{
    using IdType = unsigned int;
    const IdType nullId = 0;

    template<typename TId>
    struct Entity
    {
        TId id;
        Entity(TId id):
            id(id)
        {}
    };

    struct Camera: public Entity<IdType>
    {
        Camera(IdType id):
            Entity(id)
        {}
    };

    struct Window: public Entity<IdType>
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

    struct View: public Entity<IdType>
    {
        IdType windowId;
        IdType cameraId;
        Viewport viewport;

        View(IdType id, IdType windowId, IdType cameraId, Viewport viewport):
            Entity(id),
            windowId(windowId),
            cameraId(cameraId),
            viewport(viewport)
        {}
    };

    struct PipelineNode: Entity<IdType>
    {
        std::shared_ptr<void> attachedController;
        IdType parentNodeId;
        std::string name;
        int priority;

        PipelineNode(IdType id, std::shared_ptr<void> attachedController = nullptr, int priority = 0, IdType parentNodeId = nullId, std::string name = ""):
            Entity(id),
            attachedController(attachedController),
            parentNodeId(parentNodeId),
            priority(priority),
            name(name)
        {}
    };
}