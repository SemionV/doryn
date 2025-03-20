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
        inline int x(const int value) { return value; }
        inline int x() { return 0; }
        inline int y(const int value) { return value; }
        inline int y() { return 0; }
        inline int w(const int value) { return value; }
        inline int w() { return 0; }
        inline int h(const int value) { return value; }
        inline int h() { return 0; }
        inline std::size_t parent(const std::size_t value) { return value; }
        inline std::size_t parent() { return 0; }
        inline const std::vector<std::size_t>& kids(const std::vector<std::size_t>& value) { return value; }

        template<typename TKids>
        constexpr Container con(const Name& name, const std::size_t parent, const int x, const int y, const int width, const int height, TKids&& kids)
        {
            return Container(name, Position { x, y }, Size { width, height }, parent, std::forward<TKids>(kids));
        }

        constexpr Container con(const Name& name, const std::size_t parent, const int x, const int y, const int width, const int height)
        {
            return Container(name, Position { x, y }, Size { width, height }, parent, {});
        }
    }
}
