#pragma once

#include "../name.h"
#include <vector>

namespace dory::core::resources::entities::layout
{
    struct Position
    {
        int x{};
        int y{};
    };

    struct Size
    {
        int width{};
        int height{};
    };

    struct Container
    {
        Name name {};
        Position position {};
        Size size {};
        std::size_t parent;
        std::vector<std::size_t> children {};
    };

    struct Layout: Entity<>
    {
        std::vector<Container> containers;
    };
}
