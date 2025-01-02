#pragma once

#include <dory/generic/baseTypes.h>
#include <dory/generic/typeList.h>
#include <dory/core/resources/objects/sceneObject.h>
#include <dory/core/resources/id.h>
#include <dory/core/resources/scene/scene.h>
#include <dory/core/resources/scene/components.h>

namespace dory::core::services
{
    template<typename T>
    class IEntityComponentServiceGeneric
    {
    public:
        virtual void addComponent(resources::IdType objectId, resources::scene::Scene& scene, const T& component) = 0;
        virtual void removeComponent(resources::IdType objectId, resources::scene::Scene& scene, const T& component) = 0;
    };

    template<typename... TComponents>
    class IEntityComponentServiceBundle: public generic::Interface, public IEntityComponentServiceGeneric<TComponents>...
    {
    public:
        using ComponentTypes =  generic::TypeList<TComponents...>;

        using IEntityComponentServiceGeneric<TComponents>::addComponent...;
    };

    template<typename... TComponents>
    class IEntityComponentServiceBundle<generic::TypeList<TComponents...>>: public IEntityComponentServiceBundle<TComponents...>
    {};

    using EntityComponentTypes = generic::TypeList<
            resources::scene::components::Name,
            resources::scene::components::Scale,
            resources::scene::components::Orientation,
            resources::scene::components::Position,
            resources::scene::components::WorldTransform,
            resources::scene::components::LinearMovement,
            resources::scene::components::RotationMovement,
            resources::scene::components::Mesh,
            resources::scene::components::Material>;

    using IEntityComponentService = IEntityComponentServiceBundle<EntityComponentTypes>;

    class ISceneService: public IEntityComponentService
    {
    public:
        virtual resources::IdType addObject(resources::scene::Scene& scene, const resources::objects::SceneObject& entity) = 0;
        virtual void deleteObject(resources::scene::Scene& scene, resources::IdType objectId) = 0;
    };
}
