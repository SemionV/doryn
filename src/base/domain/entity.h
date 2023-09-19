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
        IdType attachedGroupId;
        IdType groupId;
        int priority;

        PipelineNode(IdType id, IdType groupId, std::shared_ptr<void> attachedController = nullptr, IdType attachedGroupId = 0, int priority = 0):
            Entity(id),
            attachedController(attachedController),
            groupId(groupId),
            attachedGroupId(attachedGroupId),
            priority(priority)
        {}
    };

    struct PipelineGroup: public Entity<IdType>
    {
        std::string name;
        int priority;

        PipelineGroup(IdType id, std::string name, int priority = 0):
            Entity(id),
            name(name),
            priority(priority)
        {}
    };
}