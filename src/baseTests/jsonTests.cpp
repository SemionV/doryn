#include "dependencies.h"
#include "base/serialization/jsonDeserializer.h"
#include "base/serialization/jsonSerializer.h"

using json = nlohmann::json;

TEST_CASE( "Basic", "[json]" )
{
    json ex1 = json::parse(R"(
    {
        "pi": 3.141,
        "happy": true
    })");

    REQUIRE(ex1["happy"] == true);

    auto happy = ex1["happy"];

    if(ex1.is_object())
    {
        auto& propertyName = ex1.items().begin().key();
    }
}

TEST_CASE( "Basic collection", "[.][json]" )
{
    json ex1 = json::parse(R"(
    [
        1,
        2
    ])");

    if(ex1.is_array())
    {
        for(auto i = 0; i < ex1.size(); ++i)
        {
            auto& itemJson = ex1.at(i);
            auto value = itemJson.get<int>();

            std::cout << value << "\n";
        }
    }
}

struct Mesh
{
    std::vector<std::array<float, 3>> vertices;
};

REFL_TYPE(Mesh)
    REFL_FIELD(vertices)
REFL_END

REFL_TYPE(std::optional<Mesh>)
REFL_END

struct Entity
{
    std::string name;
    std::array<float, 3> position;
    Mesh mesh;
};

REFL_TYPE(Entity)
        REFL_FIELD(name)
        REFL_FIELD(position)
        REFL_FIELD(mesh)
REFL_END

struct Scene
{
    std::string name;
    std::vector<Entity> entities;
};

REFL_TYPE(std::vector<Entity>)
REFL_END

REFL_TYPE(Scene)
        REFL_FIELD(name)
        REFL_FIELD(entities)
REFL_END

TEST_CASE( "Deserialize vector of objects", "[.][json]" )
{
    std::string meshJson = R"(
    {
        "vertices": [
            [1, 1, 1],
            [2, 2, 2]
        ]
    })";

    auto mesh = dory::typeMap::json::JsonDeserializer::deserialize<Mesh>(meshJson);
    REQUIRE(mesh.vertices.size() == 2);
    REQUIRE(mesh.vertices[0][0] == 1);
    REQUIRE(mesh.vertices[0][1] == 1);
    REQUIRE(mesh.vertices[0][2] == 1);
    REQUIRE(mesh.vertices[1][0] == 2);
    REQUIRE(mesh.vertices[1][1] == 2);
    REQUIRE(mesh.vertices[1][2] == 2);
}

TEST_CASE( "Deserialize top collection", "[.][json]" )
{
    std::string meshJson = R"(
    [
        [1, 1, 1],
        [2, 2, 2]
    ])";

    auto mesh = dory::typeMap::json::JsonDeserializer::deserialize<std::vector<std::array<float, 3>>>(meshJson);
    REQUIRE(mesh.size() == 2);
    REQUIRE(mesh[0][0] == 1);
    REQUIRE(mesh[0][1] == 1);
    REQUIRE(mesh[0][2] == 1);
    REQUIRE(mesh[1][0] == 2);
    REQUIRE(mesh[1][1] == 2);
    REQUIRE(mesh[1][2] == 2);
}

TEST_CASE( "Deserialize complex object", "[json]" )
{
    std::string json = R"(
    {
        "name": "scene1",
        "entities":[
            {
                "name": "entity1",
                "position": [1, 1, 1],
                "mesh": {
                    "vertices": [
                        [2, 2, 2],
                        [3, 3, 3]
                    ]
                }
            },
            {
                "name": "entity2",
                "position": [4, 4, 4],
                "mesh": {
                    "vertices": [
                        [5, 5, 5],
                        [6, 6, 6]
                    ]
                }
            }
        ]
    })";

    auto scene = dory::typeMap::json::JsonDeserializer::deserialize<Scene>(json);
    REQUIRE(scene.name == "scene1");
    REQUIRE(scene.entities.size() == 2);
    REQUIRE(scene.entities[0].name == "entity1");
    REQUIRE(scene.entities[0].position[0] == 1);
    REQUIRE(scene.entities[0].position[1] == 1);
    REQUIRE(scene.entities[0].position[2] == 1);
    REQUIRE(scene.entities[0].mesh.vertices.size() == 2);
    REQUIRE(scene.entities[0].mesh.vertices[0][0] == 2);
    REQUIRE(scene.entities[0].mesh.vertices[0][1] == 2);
    REQUIRE(scene.entities[0].mesh.vertices[0][2] == 2);
    REQUIRE(scene.entities[0].mesh.vertices[1][0] == 3);
    REQUIRE(scene.entities[0].mesh.vertices[1][1] == 3);
    REQUIRE(scene.entities[0].mesh.vertices[1][2] == 3);
    REQUIRE(scene.entities[1].name == "entity2");
    REQUIRE(scene.entities[1].position[0] == 4);
    REQUIRE(scene.entities[1].position[1] == 4);
    REQUIRE(scene.entities[1].position[2] == 4);
    REQUIRE(scene.entities[1].mesh.vertices.size() == 2);
    REQUIRE(scene.entities[1].mesh.vertices[0][0] == 5);
    REQUIRE(scene.entities[1].mesh.vertices[0][1] == 5);
    REQUIRE(scene.entities[1].mesh.vertices[0][2] == 5);
    REQUIRE(scene.entities[1].mesh.vertices[1][0] == 6);
    REQUIRE(scene.entities[1].mesh.vertices[1][1] == 6);
    REQUIRE(scene.entities[1].mesh.vertices[1][2] == 6);
}

TEST_CASE( "Deserialize with missing fileds", "[json]" )
{
    std::string json = R"(
    {
        "name": "scene1",
        "entities":[
            {
                "name": "entity1",
                "position": [1, 1, 1],
                "mesh": {
                    "vertices": [
                        [2, 2, 2],
                        [3]
                    ]
                }
            },
            {
                "position": [4, 4],
                "mesh": {
                }
            }
        ]
    })";

    auto scene = dory::typeMap::json::JsonDeserializer::deserialize<Scene>(json);
    REQUIRE(scene.name == "scene1");
    REQUIRE(scene.entities.size() == 2);
    REQUIRE(scene.entities[0].name == "entity1");
    REQUIRE(scene.entities[0].position[0] == 1);
    REQUIRE(scene.entities[0].position[1] == 1);
    REQUIRE(scene.entities[0].position[2] == 1);
    REQUIRE(scene.entities[0].mesh.vertices.size() == 2);
    REQUIRE(scene.entities[0].mesh.vertices[0][0] == 2);
    REQUIRE(scene.entities[0].mesh.vertices[0][1] == 2);
    REQUIRE(scene.entities[0].mesh.vertices[0][2] == 2);
    REQUIRE(scene.entities[0].mesh.vertices[1][0] == 3);
    REQUIRE(scene.entities[0].mesh.vertices[1][1] == 0);
    REQUIRE(scene.entities[0].mesh.vertices[1][2] == 0);
    REQUIRE(scene.entities[1].name.empty());
    REQUIRE(scene.entities[1].position[0] == 4);
    REQUIRE(scene.entities[1].position[1] == 4);
    REQUIRE(scene.entities[1].position[2] == 0);
    REQUIRE(scene.entities[1].mesh.vertices.empty());

}

struct Player
{
    std::string name;
    int points;
};

REFL_TYPE(Player)
        REFL_FIELD(name)
        REFL_FIELD(points)
REFL_END

TEST_CASE( "Serialize plaijn object", "[json]" )
{
    auto player = Player{"test", 12};

    auto json = dory::typeMap::json::JsonSerializer::serialize(player, 4);

    std::cout << json << std::endl;
}