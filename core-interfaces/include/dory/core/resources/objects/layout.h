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
        std::vector<Container> children {};
    };

    enum class Upstream
    {
        self,
        parent,
        fill,
        children,
    };

    struct DimensionValue
    {
        std::optional<Name> variable {};
        std::optional<float> percents {};
        std::optional<int> pixels {};
    };

    struct SizeValue: public DimensionValue
    {
        Upstream upstream { Upstream::self };
    };

    using PositionProperty = int Position::*;
    using SizeProperty = int Size::*;

    enum class AlignOrder
    {
        center
    };

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
        std::optional<AlignOrder> order { };
        DimensionValue value {};
    };

    struct AlignmentAxes
    {
        AlignmentAxis x;
        AlignmentAxis y;
    };

    using AlignmentAxisProperty = AlignmentAxis AlignmentAxes::*;

    struct Alignment
    {
        AlignmentAxes axes {};
        AlignmentStrategy strategy { AlignmentStrategy::origin };
    };

    struct StretchingAxis
    {
        SizeProperty property {};
        SizeValue value {};
        bool valuePropagation {}; //TODO: remove field
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
    };

    struct NodeItemSetup
    {
        Name name {};
        std::size_t parent {};
        std::vector<std::size_t> children{};
        Alignment alignment {};
        Stretching stretching {};
    };

    struct LineCursor
    {
        Position upperLeftCorner {};
        Position bottomRightCorner {};
    };


    struct NodeItemState
    {
        Size size {};
        Position position {};
        Size contentSize {};
        LineCursor cursor {};
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