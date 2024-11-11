#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <dory/core/services/sceneService.h>
#include <dory/core/resources/scene/scene.h>

namespace resources = dory::core::resources;
namespace objects = resources::objects;
namespace scene = resources::scene;
namespace services = dory::core::services;
namespace math = dory::math;

void assertSceneObject(const scene::Object& objects, resources::IdType id, std::size_t objectsCount, std::size_t index, resources::IdType parentId,
                       math::Vector3f& position, math::Vector3f& scale, const std::string& name, resources::IdType meshId)
{
    EXPECT_FALSE(id == resources::nullId);
    EXPECT_EQ(objectsCount, objects.id.size());
    auto actualIndex = objects.idToIndex.at(id);
    EXPECT_EQ(index, actualIndex);
    EXPECT_EQ(parentId, objects.parent[index]);
    EXPECT_EQ(position, objects.position[index]);
    EXPECT_EQ(scale, objects.scale[index]);
    EXPECT_EQ(name, objects.name[index]);
    EXPECT_EQ(meshId, objects.mesh[index]);
}

TEST(SceneService, addObject)
{
    auto sceneService = services::SceneService{};
    auto scene = scene::Scene{{}, "test", {}};
    auto soldierMesh = resources::IdType{1};
    auto horseMesh = resources::IdType{2};

    auto position = math::Vector3f{-1, -1, 1};
    auto scale = math::Vector3f{1, 1, 1};
    auto object = objects::SceneObject{"camera", resources::nullId, position, scale, resources::nullId};
    auto cameraId = sceneService.addObject(scene, object);
    assertSceneObject(scene.objects, cameraId, 1, 0, resources::nullId, position, scale, "camera", resources::nullId);

    position = math::Vector3f{0, 0, 0};
    scale = math::Vector3f{1, 1, 1};
    object = objects::SceneObject{"root", resources::nullId, position, scale, resources::nullId};
    auto rootId = sceneService.addObject(scene, object);
    assertSceneObject(scene.objects, rootId, 2, 1, resources::nullId, position, scale, "root", resources::nullId);

    position = math::Vector3f{-1, -1, 0};
    scale = math::Vector3f{2, 2, 2};
    object = objects::SceneObject{"soldier1", rootId, position, scale, soldierMesh};
    auto soldier1Id = sceneService.addObject(scene, object);
    assertSceneObject(scene.objects, soldier1Id, 3, 2, rootId, position, scale, "soldier1", soldierMesh);

    position = math::Vector3f{1, -1, 0};
    scale = math::Vector3f{1, 1, 1};
    object = objects::SceneObject{"horse1", rootId, position, scale, horseMesh};
    auto horse1Id = sceneService.addObject(scene, object);
    assertSceneObject(scene.objects, horse1Id, 4, 3, rootId, position, scale, "horse1", horseMesh);

    position = math::Vector3f{0, 0, 1};
    scale = math::Vector3f{1, 1, 1};
    object = objects::SceneObject{"soldier2", horse1Id, position, scale, soldierMesh};
    auto soldier2Id = sceneService.addObject(scene, object);
    assertSceneObject(scene.objects, soldier2Id, 5, 4, horse1Id, position, scale, "soldier2", soldierMesh);
}

