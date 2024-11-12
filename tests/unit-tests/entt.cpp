#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <entt/entt.hpp>

struct position {
    float x;
    float y;
};

struct velocity {
    float dx;
    float dy;
};

TEST(PlayWithEnTT, basic)
{
    entt::registry registry;

    for(auto i = 0u; i < 10u; ++i) {
        const auto entity = registry.create();
        registry.emplace<position>(entity, i * 1.f, i * 1.f);
        if(i % 2 == 0) { registry.emplace<velocity>(entity, i * .1f, i * .1f); }
    }

    const auto camera = registry.create();
    registry.emplace<position>(camera, 0.f, 0.f);

    auto view = registry.view<const position, velocity>();

    for(auto entity: view) {
        auto& vel = view.get<velocity>(entity);
        std::cout << "velocity: " << vel.dx << ", " << vel.dy << std::endl;
    }

    auto viewPos = registry.view<const position>();
    for(auto [entity, pos] : viewPos.each()) {
        std::cout << "position: " << pos.x << ", " << pos.y << std::endl;
    }
}