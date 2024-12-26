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
    resources::IdType soldier3Id;
    resources::IdType horse1Id;
    resources::IdType sword1Id;
    resources::IdType soldierMesh;
    resources::IdType horseMesh;
    resources::IdType swordMesh;
};

void assertEntity(const objects::SceneObject& object, scene::EnttScene& scene, resources::IdType id)
{
    EXPECT_FALSE(id == resources::nullId);
    EXPECT_TRUE(scene.idMap.contains(id));

    auto entity = scene.idMap[id];
    auto& registry = scene.registry;
    EXPECT_TRUE(registry.valid(entity));

    EXPECT_TRUE(registry.any_of<components::Children>(entity));

    if(object.parentId != resources::nullId)
    {
        EXPECT_TRUE(registry.any_of<components::Parent>(entity));
        auto& parentComponent = registry.get<components::Parent>(entity);
        EXPECT_TRUE(registry.valid(parentComponent.entity));
        EXPECT_TRUE(registry.any_of<components::Children>(parentComponent.entity));
        auto& children = registry.get<components::Children>(parentComponent.entity);
        auto it = std::find(children.entities.begin(), children.entities.end(), entity);
        EXPECT_FALSE(it == children.entities.end());
    }
    else
    {
        EXPECT_FALSE(registry.any_of<components::Parent>(entity));
    }

    EXPECT_TRUE(registry.any_of<components::Name>(entity));
    EXPECT_EQ(object.name, registry.get<components::Name>(entity).name);
}

void buildTestScene(scene::EnttScene& scene, services::ISceneService& sceneService, TestSceneContext& sceneContext)
{
    sceneContext.soldierMesh = resources::IdType{1};
    sceneContext.horseMesh = resources::IdType{2};
    sceneContext.swordMesh = resources::IdType{3};

    auto localTransform = objects::Transform{ {0, 0, 0}, {}, {1, 1, 1} };
    auto worldTransform = objects::Transform{ {-1, -1, 1}, {}, {1, 1, 1} };
    auto object = objects::SceneObject{ "camera", resources::nullId, localTransform};
    sceneContext.cameraId = sceneService.addObject(scene, object);
    assertEntity(object, scene, sceneContext.cameraId);

    worldTransform = objects::Transform{ {0, 0, 0}, {}, {1, 1, 1} };
    object = objects::SceneObject{"root", resources::nullId, localTransform};
    sceneContext.rootId = sceneService.addObject(scene, object);
    assertEntity(object, scene, sceneContext.rootId);

    worldTransform = objects::Transform{ {-1, -1, 0}, {}, {2, 2, 2} };
    object = objects::SceneObject{"soldier1", sceneContext.rootId, localTransform};
    sceneContext.soldier1Id = sceneService.addObject(scene, object);
    assertEntity(object, scene, sceneContext.soldier1Id);

    worldTransform = objects::Transform{ {1, -1, 0}, {}, {1, 1, 1} };
    object = objects::SceneObject{"horse1", sceneContext.rootId, localTransform};
    sceneContext.horse1Id = sceneService.addObject(scene, object);
    assertEntity(object, scene, sceneContext.horse1Id);

    worldTransform = objects::Transform{ {0, 0, 1}, {}, {1, 1, 1} };
    object = objects::SceneObject{"soldier2", sceneContext.horse1Id, localTransform};
    sceneContext.soldier2Id = sceneService.addObject(scene, object);
    assertEntity(object, scene, sceneContext.soldier2Id);

    worldTransform = objects::Transform{ {1, -1, 0}, {}, {1, 1, 1} };
    object = objects::SceneObject{"sword1", sceneContext.soldier1Id, localTransform};
    sceneContext.sword1Id = sceneService.addObject(scene, object);
    assertEntity(object, scene, sceneContext.sword1Id);

    worldTransform = objects::Transform{ {0, 0, 1}, {}, {1, 1, 1} };
    object = objects::SceneObject{"soldier3", sceneContext.rootId, localTransform};
    sceneContext.soldier3Id = sceneService.addObject(scene, object);
    assertEntity(object, scene, sceneContext.soldier3Id);
}

TEST(EnttSceneService, addObject)
{
    auto sceneService = services::EnttSceneService{};
    auto scene = scene::EnttScene{{{}, "test", resources::EcsType::entt}};

    auto localTransform = objects::Transform{ {0, 0, 0}, {}, {1, 1, 1} };
    auto worldTransform = objects::Transform{ {-1, -1, 1}, {}, {1, 1, 1} };
    auto object = objects::SceneObject{"camera", resources::nullId, localTransform};

    auto id = sceneService.addObject(scene, object);
    assertEntity(object, scene, id);
}

TEST(EnttSceneService, testScene)
{
    auto sceneService = services::EnttSceneService{};
    auto scene = scene::EnttScene{{{}, "test", resources::EcsType::entt}};

    TestSceneContext sceneContext {};
    buildTestScene(scene, sceneService, sceneContext);
}

TEST(EnttSceneService, deleteObject)
{
    auto sceneService = services::EnttSceneService{};
    auto scene = scene::EnttScene{{{}, "test", resources::EcsType::entt}};

    auto localTransform = objects::Transform{ {0, 0, 0}, {}, {1, 1, 1} };
    auto worldTransform = objects::Transform{ {-1, -1, 1}, {}, {1, 1, 1} };
    auto object = objects::SceneObject{"camera", resources::nullId, localTransform};

    auto id = sceneService.addObject(scene, object);
    assertEntity(object, scene, id);

    auto& registry = scene.registry;
    auto view = registry.view<components::Name>();

    EXPECT_EQ(1, std::distance(view.begin(), view.end()));
    sceneService.deleteObject(scene, id);
    EXPECT_EQ(0, std::distance(view.begin(), view.end()));
    EXPECT_FALSE(scene.idMap.contains(id));
}

TEST(EnttSceneService, destroyTestScene)
{
    auto sceneService = services::EnttSceneService{};
    auto scene = scene::EnttScene{{{}, "test", resources::EcsType::entt}};

    TestSceneContext sceneContext {};
    buildTestScene(scene, sceneService, sceneContext);

    auto& registry = scene.registry;
    auto view = registry.view<components::Name>();
    auto initialObjectsCount = std::distance(view.begin(), view.end());
    EXPECT_TRUE(initialObjectsCount > 0);

    //Delete the horse together with the soldier on it
    auto horseEntity = scene.idMap[sceneContext.horse1Id];
    auto horseChildren = registry.get<components::Children>(horseEntity);
    EXPECT_FALSE(horseChildren.entities.empty());
    auto childEntity = horseChildren.entities[0];
    sceneService.deleteObject(scene, sceneContext.horse1Id);
    EXPECT_FALSE(registry.valid(childEntity));
    EXPECT_FALSE(registry.valid(horseEntity));
    EXPECT_FALSE(scene.idMap.contains(sceneContext.soldier2Id));
    EXPECT_FALSE(scene.idMap.contains(sceneContext.horse1Id));

    EXPECT_EQ(2, initialObjectsCount - std::distance(view.begin(), view.end()));

    auto rootEntity = scene.idMap[sceneContext.rootId];
    auto& rootChildrenComponent = registry.get<components::Children>(rootEntity);
    EXPECT_EQ(rootChildrenComponent.entities.end(), std::find(rootChildrenComponent.entities.begin(), rootChildrenComponent.entities.end(), horseEntity));

    //Delete the root object with all the rest of the children
    sceneService.deleteObject(scene, sceneContext.rootId);
    EXPECT_EQ(6, initialObjectsCount - std::distance(view.begin(), view.end()));
    EXPECT_FALSE(scene.idMap.contains(sceneContext.sword1Id));
    EXPECT_FALSE(scene.idMap.contains(sceneContext.soldier1Id));
    EXPECT_FALSE(scene.idMap.contains(sceneContext.soldier3Id));
    EXPECT_FALSE(scene.idMap.contains(sceneContext.rootId));
    EXPECT_FALSE(registry.valid(rootEntity));
}