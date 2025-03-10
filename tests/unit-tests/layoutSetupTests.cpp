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

void assertSlideNode(const objects::layout::NodeItemSetup& itemSetup, const int width, const int height)
{
    auto [x, y] = itemSetup.alignment.axes;
    auto [w, h] = itemSetup.stretching.axes;

    EXPECT_EQ(x.order, objects::layout::AlignOrder::origin);
    EXPECT_EQ(y.order, objects::layout::AlignOrder::origin);

    EXPECT_TRUE(x.value.pixels);
    EXPECT_TRUE(y.value.pixels);
    EXPECT_EQ(x.value.pixels.value(), 0);
    EXPECT_EQ(y.value.pixels.value(), 0);

    EXPECT_FALSE(w.valuePropagation);
    EXPECT_FALSE(h.valuePropagation);
    EXPECT_TRUE(w.value.pixels);
    EXPECT_TRUE(h.value.pixels);
    EXPECT_EQ(w.value.pixels.value(), width);
    EXPECT_EQ(h.value.pixels.value(), height);
}

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

void assertColumnNode(const objects::layout::NodeItemSetup& itemSetup, const Axes& axes)
{
    auto& x = itemSetup.alignment.axes.*axes.x;
    auto& y = itemSetup.alignment.axes.*axes.y;
    auto& w = itemSetup.stretching.axes.*axes.w;
    auto& h = itemSetup.stretching.axes.*axes.h;

    EXPECT_EQ(x.order, objects::layout::AlignOrder::line);
    EXPECT_EQ(y.order, objects::layout::AlignOrder::origin);

    EXPECT_TRUE(w.valuePropagation);
    EXPECT_FALSE(h.valuePropagation);
    EXPECT_EQ(h.value.upstream, objects::layout::Upstream::parent);
}

void assertColumnNode(const objects::layout::NodeItemSetup& itemSetup, const Axes& axes, const int width)
{
    const objects::layout::StretchingAxis& w = itemSetup.stretching.axes.*axes.w;

    assertColumnNode(itemSetup, axes);

    EXPECT_EQ(w.value.upstream, objects::layout::Upstream::self);
    EXPECT_TRUE(w.value.pixels);
    EXPECT_EQ(w.value.pixels.value(), width);
}

void assertColumnNode(const objects::layout::NodeItemSetup& itemSetup, const Axes& axes, const float width)
{
    const objects::layout::StretchingAxis& w = itemSetup.stretching.axes.*axes.w;

    assertColumnNode(itemSetup, axes);

    EXPECT_EQ(w.value.upstream, objects::layout::Upstream::self);
    EXPECT_TRUE(w.value.percents);
    EXPECT_EQ(w.value.percents.value(), width);
}

void assertColumnNode(const objects::layout::NodeItemSetup& itemSetup, const Axes& axes, const Name& width)
{
    const objects::layout::StretchingAxis& w = itemSetup.stretching.axes.*axes.w;

    assertColumnNode(itemSetup, axes);

    EXPECT_EQ(w.value.upstream, objects::layout::Upstream::self);
    EXPECT_TRUE(w.value.variable);
    EXPECT_EQ(w.value.variable.value(), width);
}

void assertColumnNode(const objects::layout::NodeItemSetup& itemSetup, const Axes& axes, const objects::layout::Upstream upstream)
{
    const objects::layout::StretchingAxis& w = itemSetup.stretching.axes.*axes.w;

    assertColumnNode(itemSetup, axes);

    EXPECT_EQ(w.value.upstream, upstream);
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
    assertSlideNode(itemSetup, width, height);
}

void columnSetupTest(const layout2::ContainerDefinition& rootDefinition, std::vector<layout2::ContainerDefinition>& columns, const Axes& axes, layout2::DimensionSegmentProperty w)
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
    (column3.*w).upstream = layout2::Upstream::parent;

    layout2::ContainerDefinition& column4 = columns.emplace_back();
    column4.name = "column4";
    (column4.*w).variable = "grid.columnWidth";

    services::LayoutSetupService setupService {};
    const auto [nodes] = setupService.buildSetupList(rootDefinition);

    EXPECT_EQ(nodes.size(), 5);
    const objects::layout::NodeItemSetup& column1Setup = nodes[1];
    assertNode(column1Setup, column.name, 0);
    assertColumnNode(column1Setup, axes, 100);
    const objects::layout::NodeItemSetup& column2Setup = nodes[2];
    assertNode(column2Setup, column2.name, 0);
    assertColumnNode(column2Setup, axes, 50.f);
    const objects::layout::NodeItemSetup& column3Setup = nodes[3];
    assertNode(column3Setup, column3.name, 0);
    assertColumnNode(column3Setup, axes, objects::layout::Upstream::parent);
    const objects::layout::NodeItemSetup& column4Setup = nodes[4];
    assertNode(column4Setup, column4.name, 0);
    assertColumnNode(column4Setup, axes, "grid.columnWidth");
}

TEST(LayoutSetupTests, columnsSetup)
{
    layout2::ContainerDefinition rootDefinition {};
    columnSetupTest(rootDefinition, rootDefinition.columns, columnAxes, &layout2::ContainerDefinition::width);
}

TEST(LayoutSetupTests, rowsSetup)
{
    layout2::ContainerDefinition rootDefinition {};
    columnSetupTest(rootDefinition, rootDefinition.rows, rowAxes, &layout2::ContainerDefinition::height);
}