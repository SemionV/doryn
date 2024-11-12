#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <dory/core/services/enttSceneService.h>
#include <dory/core/resources/scene/enttScene.h>
#include <dory/core/resources/scene/components.h>
#include <dory/core/resources/scene/enttComponents.h>

namespace resources = dory::core::resources;
namespace objects = resources::objects;
namespace scene = resources::scene;
namespace services = dory::core::services;
namespace math = dory::math;
namespace components = scene::components;

struct TestSceneContext
{
    resources::IdType cameraId;
    resources::IdType rootId;
    resources::IdType soldier1Id;
    resources::IdType soldier2Id;
    resources::IdType horse1Id;
    resources::IdType sword1Id;
    resources::IdType soldierMesh;
    resources::IdType horseMesh;
    resources::IdType swordMesh;
};

void assertEntity(const objects::SceneObject& object, scene::EnttScene& scene, resources::IdType id) {
    EXPECT_FALSE(id == resources::nullId);
    EXPECT_TRUE(scene.idMap.contains(id));

    auto entity = scene.idMap[id];
    auto& registry = scene.registry;
    EXPECT_TRUE(registry.valid(entity));

    EXPECT_TRUE(registry.any_of<components::Children>(entity));

    if(object.parentId != resources::nullId) {
        EXPECT_TRUE(registry.any_of<components::Parent>(entity));
        auto& parentComponent = registry.get<components::Parent>(entity);
        EXPECT_TRUE(registry.valid(parentComponent.entity));
        EXPECT_TRUE(registry.any_of<components::Children>(parentComponent.entity));
        auto& children = registry.get<components::Children>(parentComponent.entity);
        auto it = std::find(children.entities.begin(), children.entities.end(), entity);
        EXPECT_FALSE(it == children.entities.end());
    }
    else {
        EXPECT_FALSE(registry.any_of<components::Parent>(entity));
    }

    EXPECT_TRUE(registry.any_of<components::Name>(entity));
    EXPECT_EQ(object.name, registry.get<components::Name>(entity).name);
}

void buildTestScene(scene::EnttScene& scene, services::ISceneService& sceneService, TestSceneContext& sceneContext) {
    sceneContext.soldierMesh = resources::IdType{1};
    sceneContext.horseMesh = resources::IdType{2};
    sceneContext.swordMesh = resources::IdType{3};

    auto position = math::Vector3f{-1, -1, 1};
    auto scale = math::Vector3f{1, 1, 1};
    auto object = objects::SceneObject{"camera", resources::nullId, position, scale, resources::nullId};
    sceneContext.cameraId = sceneService.addObject(scene, object);
    assertEntity(object, scene, sceneContext.cameraId);

    position = math::Vector3f{0, 0, 0};
    scale = math::Vector3f{1, 1, 1};
    object = objects::SceneObject{"root", resources::nullId, position, scale, resources::nullId};
    sceneContext.rootId = sceneService.addObject(scene, object);
    assertEntity(object, scene, sceneContext.rootId);

    position = math::Vector3f{-1, -1, 0};
    scale = math::Vector3f{2, 2, 2};
    object = objects::SceneObject{"soldier1", sceneContext.rootId, position, scale, sceneContext.soldierMesh};
    sceneContext.soldier1Id = sceneService.addObject(scene, object);
    assertEntity(object, scene, sceneContext.soldier1Id);

    position = math::Vector3f{1, -1, 0};
    scale = math::Vector3f{1, 1, 1};
    object = objects::SceneObject{"horse1", sceneContext.rootId, position, scale, sceneContext.horseMesh};
    sceneContext.horse1Id = sceneService.addObject(scene, object);
    assertEntity(object, scene, sceneContext.horse1Id);

    position = math::Vector3f{0, 0, 1};
    scale = math::Vector3f{1, 1, 1};
    object = objects::SceneObject{"soldier2", sceneContext.horse1Id, position, scale, sceneContext.soldierMesh};
    sceneContext.soldier2Id = sceneService.addObject(scene, object);
    assertEntity(object, scene, sceneContext.soldier2Id);

    position = math::Vector3f{1, -1, 0};
    scale = math::Vector3f{1, 1, 1};
    object = objects::SceneObject{"sword1", sceneContext.soldier1Id, position, scale, sceneContext.swordMesh};
    sceneContext.sword1Id = sceneService.addObject(scene, object);
    assertEntity(object, scene, sceneContext.sword1Id);
}

TEST(EnttSceneService, addObject) {
    auto sceneService = services::EnttSceneService{};
    auto scene = scene::EnttScene{{{}, resources::EcsType::entt, "test"}};

    auto position = math::Vector3f{-1, -1, 1};
    auto scale = math::Vector3f{1, 1, 1};
    auto object = objects::SceneObject{"camera", resources::nullId, position, scale, resources::nullId};

    auto id = sceneService.addObject(scene, object);
    assertEntity(object, scene, id);
}

TEST(EnttSceneService, testScene) {
    auto sceneService = services::EnttSceneService{};
    auto scene = scene::EnttScene{{{}, resources::EcsType::entt, "test"}};

    TestSceneContext sceneContext {};
    buildTestScene(scene, sceneService, sceneContext);
}