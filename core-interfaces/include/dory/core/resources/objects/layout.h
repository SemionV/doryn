#pragma once

#include "../name.h"

namespace dory::core::resources::objects::layout
{
    using Vector2i = std::array<int, 2>;

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

    using PositionProperty = int Position::*;
    using SizeProperty = int Size::*;

    enum class AlignmentStrategy
    {
        origin,
        relative,
        horizontalLine,
        verticalLine,
        horizontalTiles,
        verticalTiles
    };

    struct AlignmentAxis
    {
        PositionProperty property {};
        PositionValue value {};
    };

    struct AlignmentAxes
    {
        AlignmentAxis x;
        AlignmentAxis y;
    };

    using AlignmentAxisProperty = AlignmentAxis AlignmentAxes::*;

    struct Strategy
    {
        std::array<std::size_t, 2> axes{};
        std::optional<std::array<PositionValue, 2>> fixedPosition;
        bool lineWrap {};
        bool floating {};
    };

    struct Alignment
    {
        AlignmentAxes axes {};
        AlignmentStrategy strategy { AlignmentStrategy::origin };

        std::array<PositionValue, 2> axs {};
        std::optional<Strategy> originStrategy {};
        std::optional<Strategy> lineStrategy {};
        std::optional<Strategy> tileStrategy {};
    };

    struct Alignment2
    {
        std::array<std::size_t, 2> axes{};
        std::optional<std::array<PositionValue, 2>> fixedPosition;
        bool lineWrap {};
        bool floating {};
    };

    struct StretchingAxis
    {
        SizeProperty property {};
        SizeValue value {};
    };

    struct StretchingAxes
    {
        StretchingAxis width;
        StretchingAxis height;
    };

    using StretchingAxisProperty = StretchingAxis StretchingAxes::*;

    struct Stretching
    {
        StretchingAxes axes {};

        std::array<SizeValue, 2> axs {};
    };

    struct NodeItemSetup
    {
        Name name {};
        std::size_t parent {};
        std::vector<std::size_t> children{};
        Alignment alignment {};
        Alignment2 alignment2 {};
        Stretching stretching {};
    };

    struct LineCursor
    {
        Position upperLeftCorner {};
        Position bottomRightCorner {};

        Vector2i ul {};
        Vector2i br {};
    };

    struct NodeItemState
    {
        Size size {};
        Position position {};
        LineCursor cursor {};

        Vector2i pos {};
        Vector2i dim {};
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