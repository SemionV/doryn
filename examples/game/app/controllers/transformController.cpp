#include <dory/core/registry.h>
#include "transformController.h"
#include <dory/core/resources/scene/enttComponents.h>
#include <stack>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace dory::game
{
    using namespace core;
    using namespace resources;
    using namespace scene;
    using namespace components;

    TransformController::TransformController(core::Registry& registry):
        core::DependencyResolver{ registry }
    {}

    bool TransformController::initialize(core::resources::IdType referenceId, core::resources::DataContext& context)
    {
        return true;
    }

    void TransformController::stop(core::resources::IdType referenceId, core::resources::DataContext& context)
    {
    }

    void TransformController::update(core::resources::IdType referenceId, const generic::model::TimeSpan& timeStep, core::resources::DataContext& context)
    {
        _registry.getAll<repositories::ISceneRepository, EcsType>([&](const auto& repos) {
            for(const auto& [key, value] : repos)
            {
                auto repoRef = value.lock();
                if(repoRef)
                {
                    repoRef->each([&](Scene& scene) {
                        if(scene.ecsType == core::resources::EcsType::entt)
                        {
                            auto& enttScene = (EnttScene&)scene;
                            auto& registry = enttScene.registry;
                            auto stack = std::stack<entt::entity>{};

                            auto topLevelEntitiesView = registry.view<Object>(entt::exclude<Parent>);
                            for(auto entity : topLevelEntitiesView)
                            {
                                stack.push(entity);

                                auto& worldTransform = registry.get<WorldTransform>(entity);
                                auto& scale = registry.get<Scale>(entity);
                                auto& orientation = registry.get<Orientation>(entity);
                                auto& position = registry.get<Position>(entity);

                                worldTransform.scale = scale.value;
                                worldTransform.orientation = orientation.value;
                                worldTransform.position = position.value;
                            }

                            while(!stack.empty())
                            {
                                auto entity = stack.top();
                                stack.pop();

                                auto& children = registry.get<components::Children>(entity);
                                for(const auto childEntity : children.entities)
                                {
                                    stack.push(childEntity);

                                    auto& parentWorldTransform = registry.get<WorldTransform>(entity);
                                    auto& worldTransform = registry.get<WorldTransform>(childEntity);
                                    auto& scale = registry.get<Scale>(childEntity);
                                    auto& orientation = registry.get<Orientation>(childEntity);
                                    auto& position = registry.get<Position>(childEntity);

                                    worldTransform.scale = parentWorldTransform.scale * scale.value;
                                    glm::vec3 localPosition = position.value * parentWorldTransform.scale;
                                    if(glm::length(parentWorldTransform.orientation) > 0)
                                    {
                                        worldTransform.orientation = parentWorldTransform.orientation * orientation.value;
                                        localPosition = parentWorldTransform.orientation * localPosition;
                                    }
                                    else
                                    {
                                        worldTransform.orientation = orientation.value;
                                    }

                                    worldTransform.position = parentWorldTransform.position + localPosition;
                                }
                            }
                        }
                    });
                }
            }
        });
    }
}