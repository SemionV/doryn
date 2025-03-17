#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <dory/core/resources/scene/configuration.h>
#include <dory/core/resources/objects/layout.h>
#include <dory/core/services/layoutSetupService.h>

using namespace dory;
using namespace dory::core;
using namespace dory::core::resources;
using namespace dory::core::resources::scene;
using namespace dory::core::resources::scene::configuration;

struct Axes
{
    objects::layout::AlignmentAxisProperty x;
    objects::layout::AlignmentAxisProperty y;
    objects::layout::StretchingAxisProperty w;
    objects::layout::StretchingAxisProperty h;
};

constexpr Axes columnAxes {
    &objects::layout::AlignmentAxes::x,
    &objects::layout::AlignmentAxes::y,
    &objects::layout::StretchingAxes::width,
    &objects::layout::StretchingAxes::height
};

constexpr Axes rowAxes {
    &objects::layout::AlignmentAxes::y,
    &objects::layout::AlignmentAxes::x,
    &objects::layout::StretchingAxes::height,
    &objects::layout::StretchingAxes::width
};

void assertAxis(const objects::layout::StretchingAxis& axis, const objects::layout::Upstream upstream)
{
    EXPECT_EQ(axis.value.upstream, upstream);
}

void assertAxis(const objects::layout::StretchingAxis& axis, const int size)
{
    assertAxis(axis, objects::layout::Upstream::self);
    EXPECT_TRUE(axis.value.pixels);
    EXPECT_EQ(axis.value.pixels.value(), size);
}

void assertAxis(const objects::layout::StretchingAxis& axis, const float size)
{
    assertAxis(axis, objects::layout::Upstream::self);
    EXPECT_TRUE(axis.value.percents);
    EXPECT_EQ(axis.value.percents.value(), size);
}

void assertAxis(const objects::layout::StretchingAxis& axis, const Name& size)
{
    assertAxis(axis, objects::layout::Upstream::self);
    EXPECT_TRUE(axis.value.variable);
    EXPECT_EQ(axis.value.variable.value(), size);
}

template<typename T>
void assertAxis(const objects::layout::StretchingAxis& axis, const T& size, const bool valuePropagation)
{
    assertAxis(axis, size);
    EXPECT_EQ(axis.valuePropagation, valuePropagation);
}

void assertAxis(const objects::layout::AlignmentAxis& axis, const objects::layout::AlignOrder order)
{
    EXPECT_EQ(axis.order, order);
}

void assertAxis(const objects::layout::AlignmentAxis& axis, const int position)
{
    EXPECT_TRUE(axis.value.pixels);
    EXPECT_EQ(axis.value.pixels.value(), position);
}

void assertAxis(const objects::layout::AlignmentAxis& axis, const float position)
{
    EXPECT_TRUE(axis.value.percents);
    EXPECT_EQ(axis.value.percents.value(), position);
}

void assertAxis(const objects::layout::AlignmentAxis& axis, const Name& position)
{
    EXPECT_TRUE(axis.value.variable);
    EXPECT_EQ(axis.value.variable.value(), position);
}

template<typename T>
void assertAxis(const objects::layout::AlignmentAxis& axis, const T& position, const objects::layout::AlignOrder order)
{
    assertAxis(axis, position);
    assertAxis(axis, order);
}

void assertNode(const objects::layout::NodeItemSetup& itemSetup, const Name& name, const std::size_t parentIndex)
{
    EXPECT_EQ(itemSetup.name, name);
    EXPECT_EQ(itemSetup.parent, parentIndex);

    auto [x, y] = itemSetup.alignment.axes;
    auto [w, h] = itemSetup.stretching.axes;

    EXPECT_EQ(x.property, &objects::layout::Position::x);
    EXPECT_EQ(y.property, &objects::layout::Position::y);

    EXPECT_EQ(w.property, &objects::layout::Size::width);
    EXPECT_EQ(h.property, &objects::layout::Size::height);
}

void assertColumnNode(const objects::layout::NodeItemSetup& itemSetup, const Axes& axes, const objects::layout::AlignmentStrategy alignmentStrategy)
{
    EXPECT_EQ(itemSetup.alignment.strategy, alignmentStrategy);
    auto& h = itemSetup.stretching.axes.*axes.h;
    assertAxis(h, objects::layout::Upstream::parent, false);
}

void assertColumnNode(const objects::layout::NodeItemSetup& itemSetup, const Axes& axes, const int width, const objects::layout::AlignmentStrategy alignmentStrategy)
{
    assertColumnNode(itemSetup, axes, alignmentStrategy);
    const objects::layout::StretchingAxis& w = itemSetup.stretching.axes.*axes.w;
    assertAxis(w, width, true);
}

void assertColumnNode(const objects::layout::NodeItemSetup& itemSetup, const Axes& axes, const float width, const objects::layout::AlignmentStrategy alignmentStrategy)
{
    assertColumnNode(itemSetup, axes, alignmentStrategy);
    const objects::layout::StretchingAxis& w = itemSetup.stretching.axes.*axes.w;
    assertAxis(w, width, true);
}

void assertColumnNode(const objects::layout::NodeItemSetup& itemSetup, const Axes& axes, const Name& width, const objects::layout::AlignmentStrategy alignmentStrategy)
{
    assertColumnNode(itemSetup, axes, alignmentStrategy);
    const objects::layout::StretchingAxis& w = itemSetup.stretching.axes.*axes.w;
    assertAxis(w, width, true);
}

void assertColumnNode(const objects::layout::NodeItemSetup& itemSetup, const Axes& axes, const objects::layout::Upstream upstream, const objects::layout::AlignmentStrategy alignmentStrategy)
{
    assertColumnNode(itemSetup, axes, alignmentStrategy);
    const objects::layout::StretchingAxis& w = itemSetup.stretching.axes.*axes.w;
    assertAxis(w, upstream, true);
}

TEST(LayoutSetupTests, screenSetup)
{
    layout2::ContainerDefinition rootDefinition {};
    constexpr int width = 1024;
    constexpr int height = 768;
    rootDefinition.width = layout2::DimensionSegment{ { width } };
    rootDefinition.height = layout2::DimensionSegment{ { height } };
    rootDefinition.name = "screen";

    services::LayoutSetupService setupService {};
    const auto [nodes] = setupService.buildSetupList(rootDefinition);

    EXPECT_EQ(nodes.size(), 1);
    const objects::layout::NodeItemSetup& itemSetup = nodes[0];
    assertNode(itemSetup, rootDefinition.name, 0);

    auto [x, y] = itemSetup.alignment.axes;
    auto [w, h] = itemSetup.stretching.axes;

    EXPECT_EQ(itemSetup.alignment.strategy, objects::layout::AlignmentStrategy::origin);
    assertAxis(w, width, false);
    assertAxis(h, height, false);
}

void columnSetupTest(const layout2::ContainerDefinition& rootDefinition, std::vector<layout2::ContainerDefinition>& columns, const Axes& axes, const layout2::DimensionSegmentProperty w,
    const objects::layout::AlignmentStrategy alignmentStrategy)
{
    columns.reserve(4);

    layout2::ContainerDefinition& column = columns.emplace_back();
    column.name = "column1";
    (column.*w).pixels = 100;

    layout2::ContainerDefinition& column2 = columns.emplace_back();
    column2.name = "column2";
    (column2.*w).percents = 50.f;

    layout2::ContainerDefinition& column3 = columns.emplace_back();
    column3.name = "column3";
    (column3.*w).upstream = layout2::Upstream::fill;

    layout2::ContainerDefinition& column4 = columns.emplace_back();
    column4.name = "column4";
    (column4.*w).variable = "grid.columnWidth";

    services::LayoutSetupService setupService {};
    const auto [nodes] = setupService.buildSetupList(rootDefinition);

    EXPECT_EQ(nodes.size(), 5);
    const objects::layout::NodeItemSetup& column1Setup = nodes[1];
    assertNode(column1Setup, column.name, 0);
    assertColumnNode(column1Setup, axes, 100, alignmentStrategy);
    const objects::layout::NodeItemSetup& column2Setup = nodes[2];
    assertNode(column2Setup, column2.name, 0);
    assertColumnNode(column2Setup, axes, 50.f, alignmentStrategy);
    const objects::layout::NodeItemSetup& column4Setup = nodes[3];
    assertNode(column4Setup, column4.name, 0);
    assertColumnNode(column4Setup, axes, "grid.columnWidth", alignmentStrategy);
    const objects::layout::NodeItemSetup& column3Setup = nodes[4]; //flexible column must be las in the sequence of columns
    assertNode(column3Setup, column3.name, 0);
    assertColumnNode(column3Setup, axes, objects::layout::Upstream::fill, alignmentStrategy);

    const auto& rootNode = nodes[0];

    EXPECT_EQ(rootNode.children.size(), 4);
    EXPECT_EQ(rootNode.children[0], 1);
    EXPECT_EQ(rootNode.children[1], 2);
    EXPECT_EQ(rootNode.children[2], 4);
    EXPECT_EQ(rootNode.children[3], 3);
}

TEST(LayoutSetupTests, columnsSetup)
{
    layout2::ContainerDefinition rootDefinition {};
    columnSetupTest(rootDefinition, rootDefinition.columns, columnAxes, &layout2::ContainerDefinition::width, objects::layout::AlignmentStrategy::horizontalLine);
}

TEST(LayoutSetupTests, rowsSetup)
{
    layout2::ContainerDefinition rootDefinition {};
    columnSetupTest(rootDefinition, rootDefinition.rows, rowAxes, &layout2::ContainerDefinition::height, objects::layout::AlignmentStrategy::verticalLine);
}

void tileRowSetupTest(const layout2::ContainerDefinition& rootDefinition, std::vector<layout2::ContainerDefinition>& tiles, const Axes& axes,
    const layout2::DimensionSegmentProperty w, const layout2::DimensionSegmentProperty h, const objects::layout::AlignmentStrategy alignmentStrategy)
{
    tiles.reserve(2);

    layout2::ContainerDefinition& tile = tiles.emplace_back();
    tile.name = "tile1";
    (tile.*w).pixels = 10;
    (tile.*h).pixels = 10;

    layout2::ContainerDefinition& tile2 = tiles.emplace_back();
    tile2.name = "tile2";
    (tile2.*w).upstream = layout2::Upstream::children;
    (tile2.*h).upstream = layout2::Upstream::children;

    services::LayoutSetupService setupService {};
    const auto [nodes] = setupService.buildSetupList(rootDefinition);

    EXPECT_EQ(nodes.size(), 3);

    const objects::layout::NodeItemSetup& tile1Setup = nodes[1];
    EXPECT_EQ(tile1Setup.alignment.strategy, alignmentStrategy);
    assertNode(tile1Setup, tile.name, 0);
    assertAxis(tile1Setup.stretching.axes.*axes.w, 10, false);
    assertAxis(tile1Setup.stretching.axes.*axes.h, 10, false);

    const objects::layout::NodeItemSetup& tile2Setup = nodes[2];
    EXPECT_EQ(tile2Setup.alignment.strategy, alignmentStrategy);
    assertNode(tile2Setup, tile2.name, 0);
    assertAxis(tile2Setup.stretching.axes.*axes.w, objects::layout::Upstream::children, false);
    assertAxis(tile2Setup.stretching.axes.*axes.h, objects::layout::Upstream::children, false);
}

TEST(LayoutSetupTests, rowTilesSetup)
{
    layout2::ContainerDefinition rootDefinition {};
    tileRowSetupTest(rootDefinition, rootDefinition.tileRow, columnAxes, &layout2::ContainerDefinition::width, &layout2::ContainerDefinition::height,
        objects::layout::AlignmentStrategy::horizontalTiles);
}

TEST(LayoutSetupTests, columnTilesSetup)
{
    layout2::ContainerDefinition rootDefinition {};
    tileRowSetupTest(rootDefinition, rootDefinition.tileColumn, rowAxes, &layout2::ContainerDefinition::height, &layout2::ContainerDefinition::width,
        objects::layout::AlignmentStrategy::verticalTiles);
}

TEST(LayoutSetupTests, floatingSetup)
{
    layout2::ContainerDefinition rootDefinition {};

    auto& floatings = rootDefinition.floating;
    floatings.reserve(2);

    layout2::ContainerDefinition& floating = floatings.emplace_back();
    floating.name = "floating1";
    floating.x.pixels = 100;
    floating.y.pixels = 100;
    floating.width.pixels = 10;
    floating.height.pixels = 10;

    layout2::ContainerDefinition& floating2 = floatings.emplace_back();
    floating2.name = "floating2";
    floating2.x.percents = 10.f;
    floating2.y.align = layout2::Align::center;
    floating2.width.upstream = layout2::Upstream::children;
    floating2.height.upstream = layout2::Upstream::children;

    services::LayoutSetupService setupService {};
    const auto [nodes] = setupService.buildSetupList(rootDefinition);

    EXPECT_EQ(nodes.size(), 3);

    const objects::layout::NodeItemSetup& floating1Setup = nodes[1];
    assertNode(floating1Setup, floating.name, 0);
    EXPECT_EQ(floating1Setup.alignment.strategy, objects::layout::AlignmentStrategy::relative);
    assertAxis(floating1Setup.alignment.axes.x, 100);
    assertAxis(floating1Setup.alignment.axes.y, 100);
    assertAxis(floating1Setup.stretching.axes.width, 10, false);
    assertAxis(floating1Setup.stretching.axes.width, 10, false);

    const objects::layout::NodeItemSetup& floating2Setup = nodes[2];
    assertNode(floating2Setup, floating2.name, 0);
    EXPECT_EQ(floating2Setup.alignment.strategy, objects::layout::AlignmentStrategy::relative);
    assertAxis(floating2Setup.alignment.axes.x, 10.f);
    assertAxis(floating2Setup.alignment.axes.y, objects::layout::AlignOrder::center);
    assertAxis(floating2Setup.stretching.axes.width, objects::layout::Upstream::children, false);
    assertAxis(floating2Setup.stretching.axes.width, objects::layout::Upstream::children, false);
}