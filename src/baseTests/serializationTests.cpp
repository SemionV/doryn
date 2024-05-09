#include "dependencies.h"
#include "base/serialization/jsonDeserializer.h"
#include "base/serialization/jsonSerializer.h"
#include "base/serialization/yamlDeserializer.h"
#include "base/serialization/yamlSerializer.h"

using json = nlohmann::json;

TEST_CASE( "Basic", "[json]" )
{
    json ex1 = json::parse(R"(
    {
        "pi": 3.141,
        "happy": true
    })");

    REQUIRE(ex1["happy"] == true);
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

struct Player
{
    std::string name;
    unsigned int age;
    unsigned int ranking;
};

REFL_TYPE(Player)
        REFL_FIELD(name)
        REFL_FIELD(age)
        REFL_FIELD(ranking)
REFL_END

TEST_CASE( "Deserialize vector of objects", "[json]" )
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

TEST_CASE( "Deserialize top collection", "[json]" )
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

TEST_CASE( "Serialize plaijn object", "[json]" )
{
    auto entity = Entity{"test", {1, 2, 3}, Mesh
         {
             {
                 {1, 1, 1},
                 {2, 2, 2},
                 {3, 3, 3}
             }
         }};

    auto json = dory::typeMap::json::JsonSerializer::serialize(entity, 4);
}

TEST_CASE( "Serialize/Deserialize complex object", "[json]" )
{
    auto scene = Scene {
        "scene1",
        {
            Entity {
                "entity1",
                {1, 1, 1},
                Mesh {
                    {
                        {2, 2, 2},
                        {3, 3, 3}
                    }
                }
            },
            Entity {
                "entity2",
                {4, 4, 4},
                Mesh {
                    {
                        {5, 5, 5},
                        {6, 6, 6}
                    }
                }
            }
        }
    };

    auto json = dory::typeMap::json::JsonSerializer::serialize(scene, 4);

    auto sceneDeserialized = dory::typeMap::json::JsonDeserializer::deserialize<Scene>(json);
    REQUIRE(sceneDeserialized.name == scene.name);
    REQUIRE(sceneDeserialized.entities.size() == scene.entities.size());

    for(std::size_t i = 0; i < scene.entities.size(); ++i)
    {
        auto entity = scene.entities[i];
        auto entityDeserilized = sceneDeserialized.entities[i];
        REQUIRE(entityDeserilized.position[0] == entity.position[0]);
        REQUIRE(entityDeserilized.position[1] == entity.position[1]);
        REQUIRE(entityDeserilized.position[2] == entity.position[2]);

        REQUIRE(entityDeserilized.mesh.vertices.size() == entity.mesh.vertices.size());
        for(std::size_t j = 0; j < entity.mesh.vertices.size(); ++j)
        {
            auto vertex = entity.mesh.vertices[j];
            auto vertexDeserilized = entityDeserilized.mesh.vertices[j];

            REQUIRE(vertexDeserilized[0] == vertex[0]);
            REQUIRE(vertexDeserilized[1] == vertex[1]);
            REQUIRE(vertexDeserilized[2] == vertex[2]);
        }
    }
}

TEST_CASE( "Deserialize YAML", "[yaml]" )
{
    char yml_buf[] = "{foo: 1, bar: [2, 3], john: doe}";
    ryml::Tree tree = ryml::parse_in_place(yml_buf);

    ryml::ConstNodeRef root = tree.rootref();  // a const node reference
    ryml::ConstNodeRef bar = tree["bar"];
    REQUIRE(root.is_map());
    REQUIRE(bar.is_seq());

    REQUIRE(root["foo"].val() == "1");
    REQUIRE(root["foo"].key().str == yml_buf + 1);
    REQUIRE(bar[0].val() == "2");
    REQUIRE(root["john"].val() == "doe");
}

TEST_CASE( "Deserialize YAML map", "[yaml]" )
{
    std::string yaml = R"(
    name: Test
    age: 18
    ranking: 5)";

    auto player = dory::typeMap::yaml::YamlDeserializer::deserialize<Player>(yaml);
    REQUIRE(player.name == "Test");
    REQUIRE(player.age == 18);
    REQUIRE(player.ranking == 5);
}

TEST_CASE( "Deserialize YAML with missing fields", "[yaml]" )
{
    std::string yaml = R"(
    name: Test
    age: 18)";

    auto player = Player{};
    player.ranking = 1;
    dory::typeMap::yaml::YamlDeserializer::deserialize(yaml, player);
    REQUIRE(player.name == "Test");
    REQUIRE(player.age == 18);
    REQUIRE(player.ranking == 1);
}

TEST_CASE( "Deserialize YAML collection", "[yaml]" )
{
    std::string yaml = R"(
    - name: Test
      age: 18
      ranking: 5
    - name: Test2
      age: 38
      ranking: 2)";

    auto players = dory::typeMap::yaml::YamlDeserializer::deserialize<std::array<Player, 2>>(yaml);
    REQUIRE(players.size() == 2);
    auto& player = players[0];
    REQUIRE(player.name == "Test");
    REQUIRE(player.age == 18);
    REQUIRE(player.ranking == 5);
    auto& player2 = players[1];
    REQUIRE(player2.name == "Test2");
    REQUIRE(player2.age == 38);
    REQUIRE(player2.ranking == 2);
}

TEST_CASE( "Deserialize YAML dynamic collection", "[yaml]" )
{
    std::string yaml = R"(
    - name: Test
      age: 18
      ranking: 5
    - name: Test2
      age: 38
      ranking: 2)";

    auto players = dory::typeMap::yaml::YamlDeserializer::deserialize<std::vector<Player>>(yaml);
    REQUIRE(players.size() == 2);
    auto& player = players[0];
    REQUIRE(player.name == "Test");
    REQUIRE(player.age == 18);
    REQUIRE(player.ranking == 5);
    auto& player2 = players[1];
    REQUIRE(player2.name == "Test2");
    REQUIRE(player2.age == 38);
    REQUIRE(player2.ranking == 2);
}

TEST_CASE( "Deserialize YAML complex object", "[yaml]" )
{
    std::string yaml = R"(
    name: scene1
    entities:
      - name: entity1
        position: [1, 1, 1]
        mesh:
          vertices:
            - [2, 2, 2]
            - [3, 3, 3]
      - name: entity2
        position: [4, 4, 4]
        mesh:
          vertices:
            - [5, 5, 5]
            - [6, 6, 6])";

    auto scene = dory::typeMap::yaml::YamlDeserializer::deserialize<Scene>(yaml);
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

TEST_CASE( "Serialize YAML map", "[yaml]" )
{
    std::string yamlExpected = "name: Test\nage: 18\nranking: 5\n";

    auto player = Player{"Test", 18, 5};

    auto yaml = dory::typeMap::yaml::YamlSerializer::serialize(player);
    REQUIRE(!yaml.empty());
    REQUIRE(yaml == yamlExpected);
}

TEST_CASE( "Serialize YAML collection", "[yaml]" )
{
    std::string yamlExpected = "- name: Test\n  age: 18\n  ranking: 5\n- name: Test2\n  age: 38\n  ranking: 2\n";

    auto players = std::array<Player, 2>{
        Player{"Test", 18, 5},
        Player{"Test2", 38, 2}
    };

    auto yaml = dory::typeMap::yaml::YamlSerializer::serialize(players);
    REQUIRE(!yaml.empty());
    REQUIRE(yaml == yamlExpected);
}

TEST_CASE( "Serialize YAML dynamic collection", "[yaml]" )
{
    std::string yamlExpected = "- name: Test\n  age: 18\n  ranking: 5\n- name: Test2\n  age: 38\n  ranking: 2\n";

    auto players = std::vector<Player>{
        Player{"Test", 18, 5},
        Player{"Test2", 38, 2}
    };

    auto yaml = dory::typeMap::yaml::YamlSerializer::serialize(players);
    REQUIRE(!yaml.empty());
    REQUIRE(yaml == yamlExpected);
}

TEST_CASE( "Serialize YAML complext object", "[yaml]" )
{
    std::string yamlExpected = "name: scene1\nentities:\n  - name: entity1\n    position:\n      - 1\n      - 1\n      - 1\n    mesh:\n      vertices:\n        - - 2\n          - 2\n          - 2\n        - - 3\n          - 3\n          - 3\n  - name: entity2\n    position:\n      - 4\n      - 4\n      - 4\n    mesh:\n      vertices:\n        - - 5\n          - 5\n          - 5\n        - - 6\n          - 6\n          - 6\n";

    auto scene = Scene {
        "scene1",
        {
            Entity {
                "entity1",
                {1, 1, 1},
                Mesh {
                    {
                        {2, 2, 2},
                        {3, 3, 3}
                    }
                }
            },
            Entity {
                "entity2",
                {4, 4, 4},
                Mesh {
                    {
                        {5, 5, 5},
                        {6, 6, 6}
                    }
                }
            }
        }
    };

    auto yaml = dory::typeMap::yaml::YamlSerializer::serialize(scene);
    REQUIRE(!yaml.empty());
    REQUIRE(yaml == yamlExpected);
}