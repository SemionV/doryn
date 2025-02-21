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
    };

    struct Column;
    struct Row;
    struct DetachedContainer;

    struct Row: public Container
    {
        std::vector<Column> columns {};
        std::vector<DetachedContainer> detachedContainers {};
    };

    struct Column: public Container
    {
        std::vector<Row> rows {};
    };

    struct DetachedContainer: public Container
    {
        std::vector<Row> rows {};
    };
}