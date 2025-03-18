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

void assertAxis(const objects::layout::DimensionValue& value, const int size)
{
    ASSERT_TRUE(value.pixels);
    EXPECT_EQ(value.pixels.value(), size);
}

void assertAxis(const objects::layout::DimensionValue& value, const float size)
{
    ASSERT_TRUE(value.percents);
    EXPECT_EQ(value.percents.value(), size);
}

void assertAxis(const objects::layout::DimensionValue& value, const Name& size)
{
    ASSERT_TRUE(value.variable);
    EXPECT_EQ(value.variable.value(), size);
}

void assertAxis(const objects::layout::PositionValue& value, const objects::layout::AlignOrder order)
{
    EXPECT_EQ(*value.order, order);
}

void assertAxis(const objects::layout::SizeValue& value, const objects::layout::Upstream upstream)
{
    EXPECT_TRUE(value.upstream);
    EXPECT_EQ(*value.upstream, upstream);
}

template<typename T>
void assertAxis(const objects::layout::PositionValue& axis, const T& position, const objects::layout::AlignOrder order)
{
    assertAxis(axis, position);
    assertAxis(axis, order);
}

template<typename T>
void assertAxis(const objects::layout::SizeValue& axis, const T& size, const objects::layout::Upstream upstream)
{
    assertAxis(axis, size);
    assertAxis(axis, upstream);
}

void assertNode(const objects::layout::NodeItemSetup& itemSetup, const Name& name, const std::size_t parentIndex)
{
    EXPECT_EQ(itemSetup.name, name);
    EXPECT_EQ(itemSetup.parent, parentIndex);
}

void assertColumnNode(const objects::layout::NodeItemSetup& itemSetup, const objects::layout::Axes2D& axes)
{
    assertAxis(itemSetup.stretching.axs[axes[objects::layout::Axes::y]], objects::layout::Upstream::parent);

    EXPECT_FALSE(itemSetup.alignment2.floating);
    EXPECT_FALSE(itemSetup.alignment2.lineWrap);
    EXPECT_FALSE(itemSetup.alignment2.fixedPosition);
    EXPECT_EQ(itemSetup.alignment2.axes[objects::layout::Axes::x], axes[objects::layout::Axes::x]);
    EXPECT_EQ(itemSetup.alignment2.axes[objects::layout::Axes::y], axes[objects::layout::Axes::y]);
}

template<typename T>
void assertColumnNode(const objects::layout::NodeItemSetup& itemSetup, const objects::layout::Axes2D& axes, const T& width)
{
    assertColumnNode(itemSetup, axes);
    assertAxis(itemSetup.stretching.axs[axes[objects::layout::Axes::x]], width);
}

template<typename T, typename U>
void assertTileNode(const objects::layout::NodeItemSetup& itemSetup, const objects::layout::Axes2D& axes, const T& width, const U& height)
{
    assertAxis(itemSetup.stretching.axs[axes[objects::layout::Axes::x]], width);
    assertAxis(itemSetup.stretching.axs[axes[objects::layout::Axes::y]], height);

    EXPECT_FALSE(itemSetup.alignment2.floating);
    EXPECT_TRUE(itemSetup.alignment2.lineWrap);
    EXPECT_FALSE(itemSetup.alignment2.fixedPosition);
    EXPECT_EQ(itemSetup.alignment2.axes[objects::layout::Axes::x], axes[objects::layout::Axes::x]);
    EXPECT_EQ(itemSetup.alignment2.axes[objects::layout::Axes::y], axes[objects::layout::Axes::y]);
}

template<typename Tx, typename Ty, typename Tw, typename Th>
void assertFloatingNode(const objects::layout::NodeItemSetup& itemSetup, const objects::layout::Axes2D& axes, const Tx& x, const Ty& y, const Tw& width, const Th& height)
{
    const auto& alignment = itemSetup.alignment2;
    ASSERT_TRUE(alignment.fixedPosition);
    auto positionValues = alignment.fixedPosition.value();
    assertAxis(positionValues[axes[objects::layout::Axes::x]], x);
    assertAxis(positionValues[axes[objects::layout::Axes::y]], y);
    assertAxis(itemSetup.stretching.axs[axes[objects::layout::Axes::x]], width);
    assertAxis(itemSetup.stretching.axs[axes[objects::layout::Axes::y]], height);

    EXPECT_TRUE(alignment.floating);
    EXPECT_FALSE(alignment.lineWrap);
    EXPECT_EQ(alignment.axes[objects::layout::Axes::x], axes[objects::layout::Axes::x]);
    EXPECT_EQ(alignment.axes[objects::layout::Axes::y], axes[objects::layout::Axes::y]);
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
    assertAxis(itemSetup.stretching.axs[objects::layout::Axes::x], width);
    assertAxis(itemSetup.stretching.axs[objects::layout::Axes::y], height);
}

void columnSetupTest(const layout2::ContainerDefinition& rootDefinition, std::vector<layout2::ContainerDefinition>& columns, const objects::layout::Axes2D& axes)
{
    columns.reserve(4);

    layout2::ContainerDefinition& column = columns.emplace_back();
    column.name = "column1";
    column.getSize(axes[objects::layout::Axes::x]).pixels = 100;

    layout2::ContainerDefinition& column2 = columns.emplace_back();
    column2.name = "column2";
    column2.getSize(axes[objects::layout::Axes::x]).percents = 50.f;

    layout2::ContainerDefinition& column3 = columns.emplace_back();
    column3.name = "column3";
    column3.getSize(axes[objects::layout::Axes::x]).upstream = layout2::Upstream::fill;

    layout2::ContainerDefinition& column4 = columns.emplace_back();
    column4.name = "column4";
    column4.getSize(axes[objects::layout::Axes::x]).variable = "grid.columnWidth";

    services::LayoutSetupService setupService {};
    const auto [nodes] = setupService.buildSetupList(rootDefinition);

    EXPECT_EQ(nodes.size(), 5);
    const objects::layout::NodeItemSetup& column1Setup = nodes[1];
    assertNode(column1Setup, column.name, 0);
    assertColumnNode(column1Setup, axes, 100);
    const objects::layout::NodeItemSetup& column2Setup = nodes[2];
    assertNode(column2Setup, column2.name, 0);
    assertColumnNode(column2Setup, axes, 50.f);
    const objects::layout::NodeItemSetup& column4Setup = nodes[3];
    assertNode(column4Setup, column4.name, 0);
    assertColumnNode(column4Setup, axes, "grid.columnWidth");
    const objects::layout::NodeItemSetup& column3Setup = nodes[4]; //flexible column must be las in the sequence of columns
    assertNode(column3Setup, column3.name, 0);
    assertColumnNode(column3Setup, axes, objects::layout::Upstream::fill);

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
    columnSetupTest(rootDefinition, rootDefinition.columns, objects::layout::Axes::xy);
}

TEST(LayoutSetupTests, rowsSetup)
{
    layout2::ContainerDefinition rootDefinition {};
    columnSetupTest(rootDefinition, rootDefinition.rows, objects::layout::Axes::yx);
}

void tileRowSetupTest(const layout2::ContainerDefinition& rootDefinition, std::vector<layout2::ContainerDefinition>& tiles, const objects::layout::Axes2D& axes)
{
    tiles.reserve(2);

    layout2::ContainerDefinition& tile = tiles.emplace_back();
    tile.name = "tile1";
    tile.getSize(axes[objects::layout::Axes::x]).pixels = 10;
    tile.getSize(axes[objects::layout::Axes::y]).pixels = 10;

    layout2::ContainerDefinition& tile2 = tiles.emplace_back();
    tile2.name = "tile2";
    tile.getSize(axes[objects::layout::Axes::x]).upstream = layout2::Upstream::children;
    tile.getSize(axes[objects::layout::Axes::y]).upstream = layout2::Upstream::children;

    services::LayoutSetupService setupService {};
    const auto [nodes] = setupService.buildSetupList(rootDefinition);

    EXPECT_EQ(nodes.size(), 3);

    const objects::layout::NodeItemSetup& tile1Setup = nodes[1];
    assertNode(tile1Setup, tile.name, 0);
    assertTileNode(tile1Setup, axes, 10, 10);

    const objects::layout::NodeItemSetup& tile2Setup = nodes[2];
    assertNode(tile2Setup, tile2.name, 0);
    assertTileNode(tile1Setup, axes, objects::layout::Upstream::children, objects::layout::Upstream::children);
}

TEST(LayoutSetupTests, rowTilesSetup)
{
    layout2::ContainerDefinition rootDefinition {};
    tileRowSetupTest(rootDefinition, rootDefinition.tileRow, objects::layout::Axes::xy);
}

TEST(LayoutSetupTests, columnTilesSetup)
{
    layout2::ContainerDefinition rootDefinition {};
    tileRowSetupTest(rootDefinition, rootDefinition.tileColumn, objects::layout::Axes::yx);
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
    assertFloatingNode(floating1Setup, objects::layout::Axes::xy, 100, 100, 10, 10);

    const objects::layout::NodeItemSetup& floating2Setup = nodes[2];
    assertNode(floating2Setup, floating2.name, 0);
    assertFloatingNode(floating2Setup, objects::layout::Axes::xy, 10.f, objects::layout::AlignOrder::center,
        objects::layout::Upstream::children, objects::layout::Upstream::children);
}