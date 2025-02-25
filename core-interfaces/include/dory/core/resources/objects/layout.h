#pragma once

#include "../name.h"

namespace dory::core::resources::objects::layout
{
    struct Position
    {
        std::size_t x{};
        std::size_t y{};
    };

    struct Size
    {
        std::size_t width{};
        std::size_t height{}; //if it has no value use content height
    };

    struct Container
    {
        Name name {};
        Position position {};
        Size size {};
        Size actualSize {};
        std::vector<Container> children {};
        std::vector<Container> detachedChildren {};
    };
}