#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <dory/core/resources/scene/configuration.h>
#include <dory/core/resources/objects/layout.h>
#include <dory/core/services/layoutService.h>
#include <dory/core/services/layoutSetupService.h>
#include <dory/core/repositories/iLayoutRepository.h>
#include <dory/core/registry.h>

#include "mocks/entityRepository.h"

using namespace dory;
using namespace dory::core;
using namespace dory::core::resources;
using namespace dory::core::resources::scene;
using namespace dory::core::resources::scene::configuration;
using namespace dory::core::resources::entities::layout;

class LayoutRepository: public EntityRepositoryMock<repositories::ILayoutRepository>
{};

void assertContainer(const Container& container, const Name& name, const std::size_t parentIndex, const int x, const int y,
    const int width, const int height)
{
    EXPECT_EQ(container.name, name);
    EXPECT_EQ(container.parent, parentIndex);
    EXPECT_EQ(container.size.width, width);
    EXPECT_EQ(container.size.height, height);
    EXPECT_EQ(container.position.x, x);
    EXPECT_EQ(container.position.y, y);
}

void assertContainer(const Container& container, const Name& name, const std::size_t parentIndex, const int x, const int y,
    const int width, const int height, const std::size_t childrenCount)
{
    assertContainer(container, name, parentIndex, x, y, width, height);
    EXPECT_EQ(container.children.size(), childrenCount);
}

void assertContainer(const Container& container, const std::size_t indexOffset, const Name& name, const std::size_t parentIndex, const Position& position,
    const Size& size, const std::vector<std::size_t>& children)
{
    assertContainer(container, name, parentIndex, position.x, position.y, size.width, size.height);
    ASSERT_EQ(container.children.size(), children.size());
    for(std::size_t i = 0; i < children.size(); ++i)
    {
        EXPECT_EQ(children[i] + indexOffset, container.children[i]);
    }
}

void assertContainerTree(const Layout& layout, const std::size_t index, const std::vector<Container>& expected)
{
    const auto& containers = layout.containers;

    ASSERT_GE(containers.size(), index + expected.size());

    for(std::size_t i = 0; i < expected.size(); ++i)
    {
        const auto& [name, position, size, parent, children] = expected[i];
        const auto& container = containers[index + i];

        std::size_t parentIndex = parent;
        if(i > 0)
        {
            parentIndex += index;
        }

        assertContainer(container, index, name, parentIndex, position, size, children);
    }
}

void testLayout(const layout::ContainerDefinition& definition, const std::vector<Container>& expected, const std::size_t offsetIndex = 0)
{
    services::LayoutSetupService setupService {};
    const auto setupList = setupService.buildSetupList(definition);

    Layout layout {};
    services::LayoutService layoutService;
    layoutService.buildLayout(setupList, objects::layout::Variables{}, layout);

    assertContainerTree(layout, offsetIndex, expected);
}

void testWindow(const int screenWidth, const int screenHeight, const int x, const int y, const int width, const int height,
    const layout::ContainerDefinition& definition)
{
    layout::ContainerDefinition screenDefinition {};
    screenDefinition.width.pixels = screenWidth;
    screenDefinition.height.pixels = screenHeight;
    screenDefinition.name = "screen";

    screenDefinition.floating.emplace_back(definition);

    testLayout(screenDefinition, {
        Container { screenDefinition.name, Position { 0, 0 }, Size { screenWidth, screenHeight }, 0, { 1 } },
        Container { definition.name, Position { x, y }, Size { width, height }, 0, {} }
    });
}

TEST(LayoutTests, relativePosition)
{
    constexpr int x = 30;
    constexpr int y = 10;
    constexpr int width = 150;
    constexpr int height = 100;

    layout::ContainerDefinition definition;
    definition.name = "window";
    definition.x.pixels = x;
    definition.y.pixels = y;
    definition.width.pixels = width;
    definition.height.pixels = height;

    testWindow(1024, 768, x, y, width, height, definition);
}

TEST(LayoutTests, centeredPosition)
{
    constexpr int width = 150;
    constexpr int height = 100;

    layout::ContainerDefinition definition;
    definition.name = "window";
    definition.x.align = layout::Align::center;
    definition.y.align = layout::Align::center;
    definition.width.pixels = width;
    definition.height.pixels = height;

    testWindow(350, 400, 100, 150, width, height, definition);
}

TEST(LayoutTests, originPosition)
{
    constexpr int width = 150;
    constexpr int height = 100;

    layout::ContainerDefinition definition;
    definition.name = "window";
    definition.width.pixels = width;
    definition.height.pixels = height;

    testWindow(350, 400, 0, 0, width, height, definition);
}

TEST(LayoutTests, fullScreen)
{
    layout::ContainerDefinition definition;
    definition.name = "window";
    definition.width.upstream = layout::Upstream::parent;
    definition.height.upstream = layout::Upstream::parent;

    testWindow(1024, 768, 0, 0, 1024, 768, definition);
}

TEST(LayoutTests, percentDimensions)
{
    layout::ContainerDefinition definition;
    definition.name = "window";
    definition.x.percents = 10.f;
    definition.y.percents = 20.f;
    definition.width.percents = 30.f;
    definition.height.percents = 10.f;

    testWindow(1000, 1000, 100, 200, 300, 100, definition);
}

TEST(LayoutTests, rowOfTwoColumns)
{
    constexpr int windowWidth = 1024;
    constexpr int windowHeight = 768;
    constexpr int column1Width = 124;

    layout::ContainerDefinition definition;
    definition.name = "window";
    definition.width.pixels = windowWidth;
    definition.height.pixels = windowHeight;

    auto& columns = definition.columns;
    columns.resize(2);

    auto& column1Definition = columns[0];
    column1Definition.name = "column1";
    column1Definition.width.pixels = column1Width;

    auto& column2Definition = columns[1];
    column2Definition.name = "column2";
    column2Definition.width.upstream = layout::Upstream::fill;

    testLayout(definition, {
        Container { definition.name, Position { 0, 0 }, Size { windowWidth, windowHeight }, 0, { 1, 2 } },
        Container { column1Definition.name, Position { 0, 0 }, Size { column1Width, windowHeight }, 0, {} },
        Container { column2Definition.name, Position { column1Width, 0 }, Size { windowWidth - column1Width, windowHeight }, 0, {} }
    });
}

TEST(LayoutTests, rowOfTwoColumnsWithFlexibleFirst)
{
    constexpr int windowWidth = 1024;
    constexpr int windowHeight = 768;
    constexpr int column1Width = 124;

    layout::ContainerDefinition definition;
    definition.name = "window";
    definition.width.pixels = windowWidth;
    definition.height.pixels = windowHeight;

    auto& columns = definition.columns;
    columns.resize(2);

    auto& column2Definition = columns[0];
    column2Definition.name = "column2";
    column2Definition.width.upstream = layout::Upstream::fill;

    auto& column1Definition = columns[1];
    column1Definition.name = "column1";
    column1Definition.width.pixels = column1Width;

    testLayout(definition, {
        Container { definition.name, Position { 0, 0 }, Size { windowWidth, windowHeight }, 0, { 1, 2 } },
        Container { column2Definition.name, Position { 0, 0 }, Size { windowWidth - column1Width, windowHeight }, 0, {} },
        Container { column1Definition.name, Position { windowWidth - column1Width, 0 }, Size { column1Width, windowHeight }, 0, {} }
    });
}

TEST(LayoutTests, columnOfTwoRows)
{
    constexpr int windowWidth = 1024;
    constexpr int windowHeight = 768;
    constexpr int row1Height = 168;

    layout::ContainerDefinition definition;
    definition.name = "window";
    definition.width.pixels = windowWidth;
    definition.height.pixels = windowHeight;

    auto& rows = definition.rows;
    rows.resize(2);

    auto& row1Definition = rows[0];
    row1Definition.name = "row1";
    row1Definition.height.pixels = row1Height;

    auto& row2Definition = rows[1];
    row2Definition.name = "row2";
    row2Definition.height.upstream = layout::Upstream::fill;

    testLayout(definition, {
        Container { definition.name, Position { 0, 0 }, Size { windowWidth, windowHeight }, 0, { 1, 2 } },
        Container { row1Definition.name, Position { 0, 0 }, Size { windowWidth, row1Height }, 0, {} },
        Container { row2Definition.name, Position { 0, row1Height }, Size { windowWidth, windowHeight - row1Height }, 0, {} }
    });
}

TEST(LayoutTests, windowWidthStretchedByContent)
{
    constexpr int screenWidth = 1024;
    constexpr int screenHeight = 768;
    constexpr int windowHeight = 400;
    constexpr int column1Width = 100;
    constexpr int column2Width = 200;

    layout::ContainerDefinition screenDefinition {};
    screenDefinition.width.pixels = screenWidth;
    screenDefinition.height.pixels = screenHeight;
    screenDefinition.name = "screen";
    screenDefinition.floating.resize(1);

    layout::ContainerDefinition& windowDefinition = screenDefinition.floating[0];
    windowDefinition.name = "window";
    windowDefinition.x.align = layout::Align::center;
    windowDefinition.y.align = layout::Align::center;
    windowDefinition.width.upstream = layout::Upstream::children;
    windowDefinition.height.pixels = windowHeight;

    auto& columns = windowDefinition.columns;
    columns.resize(2);

    auto& column1Definition = columns[0];
    column1Definition.name = "column1";
    column1Definition.width.pixels = column1Width;

    auto& column2Definition = columns[1];
    column2Definition.name = "column2";
    column2Definition.width.pixels = column2Width;

    constexpr int expectedWindowWidth = column1Width + column2Width;
    const int expectedWindowX = static_cast<int>(std::round(static_cast<float>(screenWidth - expectedWindowWidth) / 2.f));
    const int expectedWindowY = static_cast<int>(std::round(static_cast<float>(screenHeight - windowHeight) / 2.f));

    testLayout(screenDefinition, {
        Container { screenDefinition.name, Position { 0, 0 }, Size { screenWidth, screenHeight }, 0, { 1 } },
        Container { windowDefinition.name, Position { expectedWindowX, expectedWindowY }, Size { expectedWindowWidth, windowHeight }, 0, { 2, 3 } },
        Container { column1Definition.name, Position { 0, 0 }, Size { column1Width, windowHeight }, 1, {} },
        Container { column2Definition.name, Position { column1Width, 0 }, Size { column2Width, windowHeight }, 1, {} }
    });
}

TEST(LayoutTests, windowHeightStretchedByContent)
{
    constexpr int screenWidth = 1024;
    constexpr int screenHeight = 768;
    constexpr int windowWidth = 400;
    constexpr int row1Height = 100;
    constexpr int row2Height = 200;

    layout::ContainerDefinition screenDefinition {};
    screenDefinition.width.pixels = screenWidth;
    screenDefinition.height.pixels = screenHeight;
    screenDefinition.name = "screen";
    screenDefinition.floating.resize(1);

    layout::ContainerDefinition& windowDefinition = screenDefinition.floating[0];
    windowDefinition.name = "window";
    windowDefinition.x.align = layout::Align::center;
    windowDefinition.y.align = layout::Align::center;
    windowDefinition.width.pixels = windowWidth;
    windowDefinition.height.upstream = layout::Upstream::children;

    auto& rows = windowDefinition.rows;
    rows.resize(2);

    auto& row1Definition = rows[0];
    row1Definition.name = "row1";
    row1Definition.height.pixels = row1Height;

    auto& row2Definition = rows[1];
    row2Definition.name = "row2";
    row2Definition.height.pixels = row2Height;

    constexpr int expectedWindowHeight = row1Height + row2Height;
    const int expectedWindowX = static_cast<int>(std::round(static_cast<float>(screenWidth - windowWidth) / 2.f));
    const int expectedWindowY = static_cast<int>(std::round(static_cast<float>(screenHeight - expectedWindowHeight) / 2.f));

    testLayout(screenDefinition, {
        Container { screenDefinition.name, Position { 0, 0 }, Size { screenWidth, screenHeight }, 0, { 1 } },
        Container { windowDefinition.name, Position { expectedWindowX, expectedWindowY }, Size { windowWidth, expectedWindowHeight }, 0, { 2, 3 } },
        Container { row1Definition.name, Position { 0, 0 }, Size { windowWidth, row1Height }, 1, {} },
        Container { row2Definition.name, Position { 0, row1Height }, Size { windowWidth, row2Height }, 1, {} }
    });
}

TEST(LayoutTests, rowOfThreeColumns)
{
    constexpr int windowWidth = 1024;
    constexpr int windowHeight = 768;
    constexpr int column1Width = 124;
    constexpr int column3Width = 50;

    layout::ContainerDefinition definition;
    definition.name = "window";
    definition.width.pixels = windowWidth;
    definition.height.pixels = windowHeight;

    auto& columns = definition.columns;
    columns.resize(3);

    auto& column1Definition = columns[0];
    column1Definition.name = "column1";
    column1Definition.width.pixels = column1Width;

    auto& column2Definition = columns[1];
    column2Definition.name = "column2";
    column2Definition.width.upstream = layout::Upstream::fill;

    auto& column3Definition = columns[2];
    column3Definition.name = "column3";
    column3Definition.width.pixels = column3Width;

    testLayout(definition, {
        Container { definition.name, Position { 0, 0 }, Size { windowWidth, windowHeight }, 0, { 1, 2, 3 } },
        Container { column1Definition.name, Position { 0, 0 }, Size { column1Width, windowHeight }, 0, {} },
        Container { column2Definition.name, Position { column1Width, 0 }, Size { windowWidth - column1Width - column3Width, windowHeight }, 0, {} },
        Container { column3Definition.name, Position { windowWidth - column3Width, 0 }, Size { column3Width, windowHeight }, 0, {} }
    });
}

TEST(LayoutTests, columnOfThreeRows)
{
    constexpr int windowWidth = 1024;
    constexpr int windowHeight = 768;
    constexpr int row1Height = 168;
    constexpr int row3Height = 50;

    layout::ContainerDefinition definition;
    definition.name = "window";
    definition.width.pixels = windowWidth;
    definition.height.pixels = windowHeight;

    auto& rows = definition.rows;
    rows.resize(3);

    auto& row1Definition = rows[0];
    row1Definition.name = "row1";
    row1Definition.height.pixels = row1Height;

    auto& row2Definition = rows[1];
    row2Definition.name = "row2";
    row2Definition.height.upstream = layout::Upstream::fill;

    auto& row3Definition = rows[2];
    row3Definition.name = "row3";
    row3Definition.height.pixels = row3Height;

    testLayout(definition, {
        Container { definition.name, Position { 0, 0 }, Size { windowWidth, windowHeight }, 0, { 1, 2, 3 } },
        Container { row1Definition.name, Position { 0, 0 }, Size { windowWidth, row1Height }, 0, {} },
        Container { row2Definition.name, Position { 0, row1Height }, Size { windowWidth, windowHeight - row1Height - row3Height }, 0, {} },
        Container { row3Definition.name, Position { 0, windowHeight - row3Height }, Size { windowWidth, row3Height }, 0, {} }
    });
}

TEST(LayoutTests, combinedThreeColumnAndRowsGridLayout2)
{
    constexpr int windowWidth = 1024;
    constexpr int windowHeight = 768;
    constexpr int column1Width = 124;
    constexpr int column3Width = 50;
    constexpr int row1Height = 168;
    constexpr int row3Height = 50;

    layout::ContainerDefinition definition;
    definition.name = "window";
    definition.width.pixels = windowWidth;
    definition.height.pixels = windowHeight;

    auto& columns = definition.columns;
    columns.resize(3);

    auto& column1Definition = columns[0];
    column1Definition.name = "column1";
    column1Definition.width.pixels = column1Width;

    auto& column2Definition = columns[1];
    column2Definition.name = "column2";
    column2Definition.width.upstream = layout::Upstream::fill;

    auto& rows = column2Definition.rows;
    rows.resize(3);

    auto& row1Definition = rows[0];
    row1Definition.name = "row1";
    row1Definition.height.pixels = row1Height;

    auto& row2Definition = rows[1];
    row2Definition.name = "row2";
    row2Definition.height.upstream = layout::Upstream::fill;

    auto& row3Definition = rows[2];
    row3Definition.name = "row3";
    row3Definition.height.pixels = row3Height;

    auto& column3Definition = columns[2];
    column3Definition.name = "column3";
    column3Definition.width.pixels = column3Width;

    constexpr int expectedColumn2Width = windowWidth - column1Width - column3Width;

    testLayout(definition, {
        Container { definition.name, Position { 0, 0 }, Size { windowWidth, windowHeight }, 0, { 1, 2, 6 } },
            Container { column1Definition.name, Position { 0, 0 }, Size { column1Width, windowHeight }, 0, {} },
            Container { column2Definition.name, Position { column1Width, 0 }, Size { expectedColumn2Width, windowHeight }, 0, {3, 4, 5} },
                Container { row1Definition.name, Position { 0, 0 }, Size { expectedColumn2Width, row1Height }, 2, {} },
                Container { row2Definition.name, Position { 0, row1Height }, Size { expectedColumn2Width, windowHeight - row1Height - row3Height }, 2, {} },
                Container { row3Definition.name, Position { 0, windowHeight - row3Height }, Size { expectedColumn2Width, row3Height }, 2, {} },
            Container { column3Definition.name, Position { windowWidth - column3Width, 0 }, Size { column3Width, windowHeight }, 0, {} }
    });
}

TEST(LayoutTests, scrollableContent)
{
    constexpr int windowWidth = 400;
    constexpr int windowHeight = 400;
    constexpr int contentHeight = 3000;

    layout::ContainerDefinition windowDefinition;
    windowDefinition.name = "window";
    windowDefinition.width.pixels = windowWidth;
    windowDefinition.height.pixels = windowHeight;

    windowDefinition.slides.resize(1);

    layout::ContainerDefinition& slideDefinition = windowDefinition.slides[0];
    slideDefinition.name = "slide";
    slideDefinition.width.upstream = layout::Upstream::parent;
    slideDefinition.height.pixels = contentHeight;

    testLayout(windowDefinition, {
        Container { windowDefinition.name, Position { 0, 0 }, Size { windowWidth, windowHeight }, 0, { 1 } },
        Container { slideDefinition.name, Position { 0, 0 }, Size { windowWidth, contentHeight }, 0, {} }
    });
}

TEST(LayoutTests, horizontalTiles2)
{
    constexpr int windowWidth = 350;
    constexpr int tileWidth = 100;
    constexpr int tileHeight = 100;

    layout::ContainerDefinition windowDefinition;
    windowDefinition.name = "window";
    windowDefinition.width.pixels = windowWidth;
    windowDefinition.height.upstream = layout::Upstream::children;

    auto& tilesRow = windowDefinition.tileRow;
    tilesRow.resize(5);

    for(std::size_t i = 0; i < 5; ++i)
    {
        layout::ContainerDefinition& tileDefinition = tilesRow[i];
        tileDefinition.width.pixels = 100;
        tileDefinition.height.pixels = 100;
    }

    testLayout(windowDefinition, {
        Container { windowDefinition.name, Position { 0, 0 }, Size { windowWidth, tileHeight * 2 }, 0, { 1, 2, 3, 4, 5 } },
        Container { "", Position { 0, 0 }, Size { tileWidth, tileHeight }, 0, {} },
        Container { "", Position { 100, 0 }, Size { tileWidth, tileHeight }, 0, {} },
        Container { "", Position { 200, 0 }, Size { tileWidth, tileHeight }, 0, {} },
        Container { "", Position { 0, 100 }, Size { tileWidth, tileHeight }, 0, {} },
        Container { "", Position { 100, 100 }, Size { tileWidth, tileHeight }, 0, {} }
    });
}

TEST(LayoutTests, verticalTiles2)
{
    constexpr int windowHeight = 350;
    constexpr int tileWidth = 100;
    constexpr int tileHeight = 100;

    layout::ContainerDefinition windowDefinition;
    windowDefinition.name = "window";
    windowDefinition.height.pixels = windowHeight;
    windowDefinition.width.upstream = layout::Upstream::children;

    auto& tilesColumn = windowDefinition.tileColumn;
    tilesColumn.resize(5);

    for(std::size_t i = 0; i < 5; ++i)
    {
        layout::ContainerDefinition& tileDefinition = tilesColumn[i];
        tileDefinition.width.pixels = 100;
        tileDefinition.height.pixels = 100;
    }

    testLayout(windowDefinition, {
        Container { windowDefinition.name, Position { 0, 0 }, Size { tileWidth * 2, windowHeight }, 0, { 1, 2, 3, 4, 5 } },
        Container { "", Position { 0, 0 }, Size { tileWidth, tileHeight }, 0, {} },
        Container { "", Position { 0, 100 }, Size { tileWidth, tileHeight }, 0, {} },
        Container { "", Position { 0, 200 }, Size { tileWidth, tileHeight }, 0, {} },
        Container { "", Position { 100, 0 }, Size { tileWidth, tileHeight }, 0, {} },
        Container { "", Position { 100, 100 }, Size { tileWidth, tileHeight }, 0, {} }
    });
}

//3 column layout with a flexible column in the middle, which contains another three columns with a flexible column in the middle,
//where one of the columns has width defined as a percent of the parents width
TEST(LayoutTests, threeColumnsWithThreeColumns)
{
    constexpr int windowWidth = 1024;
    constexpr int windowHeight = 768;
    constexpr int column1Width = 124;
    constexpr int column3Width = 50;
    constexpr float column1_1Width = 50.f;
    constexpr int column1_3Width = 100;

    layout::ContainerDefinition definition;
    {
        using namespace layout::util;
        definition = row("window", w(windowWidth), h(windowHeight), {
            def("column1", w(column1Width), h() ),
            row("column2", w(us::fill), h(), {
                def("column1_1", w(column1_1Width), h()),
                def("column1_2", w(us::fill), h()),
                def("column1_3", w(column1_3Width), h())
            }),
            def("column3", w(column3Width), h())
        });
    }

    constexpr int column2WidthExpected = windowWidth - column1Width - column3Width;
    constexpr int column1_1WidthExpected = column2WidthExpected * 0.01f * column1_1Width;
    constexpr int column1_2WidthExpected = column2WidthExpected - column1_1WidthExpected - column1_3Width;
    
    using namespace entities::layout::util;
    testLayout(definition, {
        con("column2", parent(), x(column1Width), y(), w(column2WidthExpected), h(windowHeight), kids({ 1, 2, 3})),
            con("column1_1", parent(), x(), y(), w(column1_1WidthExpected), h(windowHeight)),
            con("column1_2", parent(), x(column1_1WidthExpected), y(), w(column1_2WidthExpected), h(windowHeight)),
            con("column1_3", parent(), x(column1_1WidthExpected + column1_2WidthExpected), y(), w(column1_3Width), h(windowHeight))
    }, 2);
}

//TODO: unit test for layout like word-wrap text: letters make words(lines of nodes) and words can jump to next line
//TODO: test three-column layout with a left column filled with tiles vertically and taking width from it's contents, then a flexible-width column and a fixed width column
//TODO: make a test of a 0-size container filled with tiles, which have to make a column of tiles or a row of tiles accordingly if they are horizontal or vertical alignment
//TODO: unit test with a container which has columns and a floating children