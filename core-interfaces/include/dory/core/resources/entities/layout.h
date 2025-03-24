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

    namespace util
    {
        inline Container con()
        {
            return Container{};
        }

        inline Container con(const Name& name)
        {
            Container result {};
            result.name = name;
            return result;
        }

        template <typename Func>
        Container operator|(Container container, Func func)
        {
            return func(container);
        }

        inline void setValue(int& dim, const int value)
        {
            dim = value;
        }

        inline auto setPosition(int Position::*dim, const int value) {
            return [dim, value](Container& container) -> Container& {
                setValue(container.position.*dim, value);
                return container;
            };
        }

        inline auto setSize(int Size::*dim, const int value) {
            return [dim, value](Container& container) -> Container& {
                setValue(container.size.*dim, value);
                return container;
            };
        }

        inline auto _x(const int value)
        {
            return setPosition(&Position::x, value);
        }

        inline auto _y(const int value)
        {
            return setPosition(&Position::y, value);
        }

        inline auto _w(const int value)
        {
            return setSize(&Size::width, value);
        }

        inline auto _h(const int value)
        {
            return setSize(&Size::height, value);
        }

        inline auto kids(const std::vector<std::size_t>& items) {
            return [&items](Container& container) -> Container& {
                container.children = items;
                return container;
            };
        }

        inline auto parent(const std::size_t value)
        {
            return [value](Container& container) -> Container& {
                container.parent = value;
                return container;
            };
        }
    }
}
