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
        children,
    };

    struct DimensionValue
    {
        Upstream upstream { Upstream::self };
        std::optional<Name> variable {};
        std::optional<float> percents {};
        std::optional<int> pixels {};
    };

    using PositionProperty = int Position::*;
    using SizeProperty = int Size::*;

    enum class AlignOrder
    {
        center,
        line,
        origin,
        relative
    };

    struct AlignmentAxis
    {
        PositionProperty property {};
        AlignOrder order { AlignOrder::center };
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
        AlignmentAxisProperty direction {}; //priority axis for alignment
        AlignmentAxes axes {};
    };

    struct StretchingAxis
    {
        SizeProperty property {};
        DimensionValue value {};
        bool valuePropagation {};
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
        std::size_t parent {};
        std::vector<std::size_t> children{};
        Alignment alignment {};
        Stretching stretching {};
    };

    struct NodeItemState
    {
        Size size {};
        Position position {};
        Size contentSize {};
        Position cursor {};
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