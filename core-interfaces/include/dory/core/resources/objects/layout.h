#pragma once

#include "../name.h"

namespace dory::core::resources::objects::layout
{
    using Vector2i = std::array<int, 2>;
    using Axes2D = std::array<std::size_t, 2>;

    struct Axes
    {
        static constexpr std::size_t x = 0;
        static constexpr std::size_t y = 1;

        static constexpr auto xy = std::array{x, y};
        static constexpr auto yx = std::array{y, x};
    };

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
        std::vector<Container> children {};
    };

    enum class Upstream
    {
        parent,
        fill,
        children,
    };

    enum class AlignOrder
    {
        center
    };

    struct DimensionValue
    {
        std::optional<Name> variable {};
        std::optional<float> percents {};
        std::optional<int> pixels {};
    };

    struct PositionValue: public DimensionValue
    {
        std::optional<AlignOrder> order { };
    };

    struct SizeValue: public DimensionValue
    {
        std::optional<Upstream> upstream {};
    };

    struct Strategy //TODO: rename to AlignmentStrategy
    {
        std::array<std::size_t, 2> axes{};
        std::optional<std::array<PositionValue, 2>> fixedPosition;
        bool lineWrap {};
        bool floating {};
    };

    struct Alignment2 //TODO: remove 2 from the name
    {
        Axes2D axes{};
        std::optional<std::array<PositionValue, 2>> fixedPosition;
        bool lineWrap {};
        bool floating {};
    };

    struct Stretching
    {
        std::array<SizeValue, 2> axs {}; //TODO: rename to axes
    };

    struct NodeItemSetup
    {
        Name name {};
        std::size_t parent {};
        std::vector<std::size_t> children{};
        Alignment2 alignment2 {}; //TODO: remove 2 from the name
        Stretching stretching {};
    };

    struct LineCursor
    {
        Vector2i ul {}; //upper-left corner
        Vector2i br {}; //bottom-right corner
    };

    struct NodeItemState
    {
        LineCursor cursor {};
        Vector2i pos {}; //TODO: rename to position
        Vector2i dim {}; //TODO: rename to size
    };

    struct NodeSetupList
    {
        std::vector<NodeItemSetup> nodes {};
    };

    struct NodeStateList
    {
        std::vector<NodeItemState> nodes {};
    };

    //a dummy class to represent variables storage
    struct Variables
    {};
}