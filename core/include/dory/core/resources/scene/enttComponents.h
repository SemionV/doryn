#pragma once

namespace dory::core::resources::scene::components
{
    struct Parent
    {
        entt::entity entity {};
    };

    struct Children
    {
        std::vector<entt::entity> entities {};
    };
}
