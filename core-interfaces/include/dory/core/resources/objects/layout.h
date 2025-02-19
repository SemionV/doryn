#pragma once

#include "../name.h"

namespace dory::core::resources::objects::layout
{
    struct Position
    {
        int x{};
        int y{};
    };

    struct Size
    {
        int width{};
        int height{}; //if it has no value use content height
    };

    struct Container
    {
        Name name {};
        Position position {};
        Size size {};
    };

    struct Column;

    struct Row: public Container
    {
        std::vector<Container> columns {};
    };

    struct Column: public Container
    {
        std::vector<Row> rows {};
    };
}