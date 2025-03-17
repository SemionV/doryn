#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <dory/core/resources/scene/configuration.h>
#include <dory/core/resources/objects/layout.h>
#include <dory/core/services/layoutService2.h>
#include <dory/core/services/layoutSetupService.h>

using namespace dory;
using namespace dory::core;
using namespace dory::core::resources;
using namespace dory::core::resources::scene;
using namespace dory::core::resources::scene::configuration;

void assertContainer(const objects::layout::Container& container, const Name& name, const int x, const int y,
    const int width, const int height)
{
    EXPECT_EQ(container.name, name);
    EXPECT_EQ(container.size.width, width);
    EXPECT_EQ(container.size.height, height);
    EXPECT_EQ(container.position.x, x);
    EXPECT_EQ(container.position.y, y);
}

void assertContainer(const objects::layout::Container& container, const Name& name, const int x, const int y,
    const int width, const int height, const std::size_t childrenCount)
{
    assertContainer(container, name, x, y, width, height);
    EXPECT_EQ(container.children.size(), childrenCount);
}

void testWindow(const int screenWidth, const int screenHeight, const int x, const int y, const int width, const int height,
    const layout2::ContainerDefinition& definition)
{
    layout2::ContainerDefinition screenDefinition {};
    screenDefinition.width.pixels = screenWidth;
    screenDefinition.height.pixels = screenHeight;
    screenDefinition.name = "screen";

    screenDefinition.floating.emplace_back(definition);

    services::LayoutSetupService setupService {};
    const auto setupList = setupService.buildSetupList(screenDefinition);

    services::LayoutService2 layoutService;
    const auto screen = layoutService.calculate(setupList, objects::layout::Variables{});

    ASSERT_TRUE(!!screen);
    assertContainer(*screen, screenDefinition.name, 0, 0, screenWidth, screenHeight, 1);
    const auto& window = screen->children[0];
    assertContainer(window, definition.name, x, y, width, height);
}

TEST(LayoutTests, relativePosition)
{
    constexpr int x = 30;
    constexpr int y = 10;
    constexpr int width = 150;
    constexpr int height = 100;

    layout2::ContainerDefinition definition;
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

    layout2::ContainerDefinition definition;
    definition.name = "window";
    definition.x.align = layout2::Align::center;
    definition.y.align = layout2::Align::center;
    definition.width.pixels = width;
    definition.height.pixels = height;

    testWindow(350, 400, 100, 150, width, height, definition);
}

TEST(LayoutTests, originPosition)
{
    constexpr int width = 150;
    constexpr int height = 100;

    layout2::ContainerDefinition definition;
    definition.name = "window";
    definition.width.pixels = width;
    definition.height.pixels = height;

    testWindow(350, 400, 0, 0, width, height, definition);
}

TEST(LayoutTests, fullScreen)
{
    layout2::ContainerDefinition definition;
    definition.name = "window";
    definition.width.upstream = layout2::Upstream::parent;
    definition.height.upstream = layout2::Upstream::parent;

    testWindow(1024, 768, 0, 0, 1024, 768, definition);
}

TEST(LayoutTests, percentDimensions)
{
    layout2::ContainerDefinition definition;
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

    layout2::ContainerDefinition definition;
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
    column2Definition.width.upstream = layout2::Upstream::fill;

    services::LayoutSetupService setupService {};
    const auto setupList = setupService.buildSetupList(definition);

    services::LayoutService2 layoutService;
    const auto window = layoutService.calculate(setupList, objects::layout::Variables{});

    ASSERT_TRUE(!!window);
    assertContainer(*window, definition.name, 0, 0, windowWidth, windowHeight, 2);
    const auto& column1 = window->children[0];
    assertContainer(column1, column1Definition.name, 0, 0, column1Width, windowHeight);
    const auto& column2 = window->children[1];
    assertContainer(column2, column2Definition.name, column1Width, 0, windowWidth - column1Width, windowHeight);
}

TEST(LayoutTests, columnOfTwoRows)
{
    constexpr int windowWidth = 1024;
    constexpr int windowHeight = 768;
    constexpr int row1Height = 168;

    layout2::ContainerDefinition definition;
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
    row2Definition.height.upstream = layout2::Upstream::fill;

    services::LayoutSetupService setupService {};
    const auto setupList = setupService.buildSetupList(definition);

    services::LayoutService2 layoutService;
    const auto window = layoutService.calculate(setupList, objects::layout::Variables{});

    ASSERT_TRUE(!!window);
    assertContainer(*window, definition.name, 0, 0, windowWidth, windowHeight, 2);
    const auto& row1 = window->children[0];
    assertContainer(row1, row1Definition.name, 0, 0, windowWidth, row1Height);
    const auto& row2 = window->children[1];
    assertContainer(row2, row2Definition.name, 0, row1Height, windowWidth, windowHeight - row1Height);
}

TEST(LayoutTests, windowWidthStretchedByContent)
{
    constexpr int screenWidth = 1024;
    constexpr int screenHeight = 768;
    constexpr int windowHeight = 400;
    constexpr int column1Width = 100;
    constexpr int column2Width = 200;

    layout2::ContainerDefinition screenDefinition {};
    screenDefinition.width.pixels = screenWidth;
    screenDefinition.height.pixels = screenHeight;
    screenDefinition.name = "screen";
    screenDefinition.floating.resize(1);

    layout2::ContainerDefinition& windowDefinition = screenDefinition.floating[0];
    windowDefinition.name = "window";
    windowDefinition.x.align = layout2::Align::center;
    windowDefinition.y.align = layout2::Align::center;
    windowDefinition.width.upstream = layout2::Upstream::children;
    windowDefinition.height.pixels = windowHeight;

    auto& columns = windowDefinition.columns;
    columns.resize(2);

    auto& column1Definition = columns[0];
    column1Definition.name = "column1";
    column1Definition.width.pixels = column1Width;

    auto& column2Definition = columns[1];
    column2Definition.name = "column2";
    column2Definition.width.pixels = column2Width;

    services::LayoutSetupService setupService {};
    const auto setupList = setupService.buildSetupList(screenDefinition);

    services::LayoutService2 layoutService;
    const auto screen = layoutService.calculate(setupList, objects::layout::Variables{});

    ASSERT_TRUE(!!screen);
    assertContainer(*screen, screenDefinition.name, 0, 0, screenWidth, screenHeight, 1);
    const auto& window = screen->children[0];
    constexpr int expectedWindowWidth = column1Width + column2Width;
    const int expectedWindowX = static_cast<int>(std::round(static_cast<float>(screenWidth - expectedWindowWidth) / 2.f));
    const int expectedWindowY = static_cast<int>(std::round(static_cast<float>(screenHeight - windowHeight) / 2.f));
    assertContainer(window, windowDefinition.name, expectedWindowX, expectedWindowY, expectedWindowWidth, windowHeight, 2);

    const auto& column1 = window.children[0];
    assertContainer(column1, column1Definition.name, 0, 0, column1Width, windowHeight);
    const auto& column2 = window.children[1];
    assertContainer(column2, column2Definition.name, column1Width, 0, column2Width, windowHeight);
}

TEST(LayoutTests, windowHeightStretchedByContent)
{
    constexpr int screenWidth = 1024;
    constexpr int screenHeight = 768;
    constexpr int windowWidth = 400;
    constexpr int row1Height = 100;
    constexpr int row2Height = 200;

    layout2::ContainerDefinition screenDefinition {};
    screenDefinition.width.pixels = screenWidth;
    screenDefinition.height.pixels = screenHeight;
    screenDefinition.name = "screen";
    screenDefinition.floating.resize(1);

    layout2::ContainerDefinition& windowDefinition = screenDefinition.floating[0];
    windowDefinition.name = "window";
    windowDefinition.x.align = layout2::Align::center;
    windowDefinition.y.align = layout2::Align::center;
    windowDefinition.width.pixels = windowWidth;
    windowDefinition.height.upstream = layout2::Upstream::children;

    auto& rows = windowDefinition.rows;
    rows.resize(2);

    auto& row1Definition = rows[0];
    row1Definition.name = "row1";
    row1Definition.height.pixels = row1Height;

    auto& row2Definition = rows[1];
    row2Definition.name = "row2";
    row2Definition.height.pixels = row2Height;

    services::LayoutSetupService setupService {};
    const auto setupList = setupService.buildSetupList(screenDefinition);

    services::LayoutService2 layoutService;
    const auto screen = layoutService.calculate(setupList, objects::layout::Variables{});

    ASSERT_TRUE(!!screen);
    assertContainer(*screen, screenDefinition.name, 0, 0, screenWidth, screenHeight, 1);
    const auto& window = screen->children[0];
    constexpr int expectedWindowHeight = row1Height + row2Height;
    const int expectedWindowX = static_cast<int>(std::round(static_cast<float>(screenWidth - windowWidth) / 2.f));
    const int expectedWindowY = static_cast<int>(std::round(static_cast<float>(screenHeight - expectedWindowHeight) / 2.f));
    assertContainer(window, windowDefinition.name, expectedWindowX, expectedWindowY, windowWidth, expectedWindowHeight, 2);

    const auto& row1 = window.children[0];
    assertContainer(row1, row1Definition.name, 0, 0, windowWidth, row1Height);
    const auto& row2 = window.children[1];
    assertContainer(row2, row2Definition.name, 0, row1Height, windowWidth, row2Height);
}

TEST(LayoutTests, rowOfThreeColumns)
{
    constexpr int windowWidth = 1024;
    constexpr int windowHeight = 768;
    constexpr int column1Width = 124;
    constexpr int column3Width = 50;

    layout2::ContainerDefinition definition;
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
    column2Definition.width.upstream = layout2::Upstream::fill;

    auto& column3Definition = columns[2];
    column3Definition.name = "column3";
    column3Definition.width.pixels = column3Width;

    services::LayoutSetupService setupService {};
    const auto setupList = setupService.buildSetupList(definition);

    services::LayoutService2 layoutService;
    const auto window = layoutService.calculate(setupList, objects::layout::Variables{});

    ASSERT_TRUE(!!window);
    assertContainer(*window, definition.name, 0, 0, windowWidth, windowHeight, 3);
    const auto& column1 = window->children[0];
    assertContainer(column1, column1Definition.name, 0, 0, column1Width, windowHeight);
    const auto& column2 = window->children[1];
    assertContainer(column2, column2Definition.name, column1Width, 0, windowWidth - column1Width - column3Width, windowHeight);
    const auto& column3 = window->children[2];
    assertContainer(column3, column3Definition.name, windowWidth - column3Width, 0, column3Width, windowHeight);
}

TEST(LayoutTests, columnOfThreeRows)
{
    constexpr int windowWidth = 1024;
    constexpr int windowHeight = 768;
    constexpr int row1Height = 168;
    constexpr int row3Height = 50;

    layout2::ContainerDefinition definition;
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
    row2Definition.height.upstream = layout2::Upstream::fill;

    auto& row3Definition = rows[2];
    row3Definition.name = "row3";
    row3Definition.height.pixels = row3Height;

    services::LayoutSetupService setupService {};
    const auto setupList = setupService.buildSetupList(definition);

    services::LayoutService2 layoutService;
    const auto window = layoutService.calculate(setupList, objects::layout::Variables{});

    ASSERT_TRUE(!!window);
    assertContainer(*window, definition.name, 0, 0, windowWidth, windowHeight, 3);
    const auto& row1 = window->children[0];
    assertContainer(row1, row1Definition.name, 0, 0, windowWidth, row1Height);
    const auto& row2 = window->children[1];
    assertContainer(row2, row2Definition.name, 0, row1Height, windowWidth, windowHeight - row1Height - row3Height);
    const auto& row3 = window->children[2];
    assertContainer(row3, row3Definition.name, 0, windowHeight - row3Height, windowWidth, row3Height);
}

TEST(LayoutTests, combinedThreeColumnAndRowsGridLayout2)
{
    constexpr int windowWidth = 1024;
    constexpr int windowHeight = 768;
    constexpr int column1Width = 124;
    constexpr int column3Width = 50;
    constexpr int row1Height = 168;
    constexpr int row3Height = 50;

    layout2::ContainerDefinition definition;
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
    column2Definition.width.upstream = layout2::Upstream::fill;

    auto& rows = column2Definition.rows;
    rows.resize(3);

    auto& row1Definition = rows[0];
    row1Definition.name = "row1";
    row1Definition.height.pixels = row1Height;

    auto& row2Definition = rows[1];
    row2Definition.name = "row2";
    row2Definition.height.upstream = layout2::Upstream::fill;

    auto& row3Definition = rows[2];
    row3Definition.name = "row3";
    row3Definition.height.pixels = row3Height;

    auto& column3Definition = columns[2];
    column3Definition.name = "column3";
    column3Definition.width.pixels = column3Width;

    services::LayoutSetupService setupService {};
    const auto setupList = setupService.buildSetupList(definition);

    services::LayoutService2 layoutService;
    const auto window = layoutService.calculate(setupList, objects::layout::Variables{});

    ASSERT_TRUE(!!window);
    assertContainer(*window, definition.name, 0, 0, windowWidth, windowHeight, 3);
    const auto& column1 = window->children[0];
    assertContainer(column1, column1Definition.name, 0, 0, column1Width, windowHeight);
    const auto& column2 = window->children[1];
    constexpr int expectedColumn2Width = windowWidth - column1Width - column3Width;
    assertContainer(column2, column2Definition.name, column1Width, 0, expectedColumn2Width, windowHeight, 3);
    const auto& column3 = window->children[2];
    assertContainer(column3, column3Definition.name, windowWidth - column3Width, 0, column3Width, windowHeight);

    const auto& row1 = column2.children[0];
    assertContainer(row1, row1Definition.name, 0, 0, expectedColumn2Width, row1Height);
    const auto& row2 = column2.children[1];
    assertContainer(row2, row2Definition.name, 0, row1Height, expectedColumn2Width, windowHeight - row1Height - row3Height);
    const auto& row3 = column2.children[2];
    assertContainer(row3, row3Definition.name, 0, windowHeight - row3Height, expectedColumn2Width, row3Height);
}

TEST(LayoutTests, scrollableContent)
{
    constexpr int windowWidth = 400;
    constexpr int windowHeight = 400;
    constexpr int contentHeight = 3000;

    layout2::ContainerDefinition windowDefinition;
    windowDefinition.name = "window";
    windowDefinition.width.pixels = windowWidth;
    windowDefinition.height.pixels = windowHeight;

    windowDefinition.slides.resize(1);

    layout2::ContainerDefinition& slideDefinition = windowDefinition.slides[0];
    slideDefinition.name = "slide";
    slideDefinition.width.upstream = layout2::Upstream::parent;
    slideDefinition.height.pixels = contentHeight;

    services::LayoutSetupService setupService {};
    const auto setupList = setupService.buildSetupList(windowDefinition);

    services::LayoutService2 layoutService;
    const auto window = layoutService.calculate(setupList, objects::layout::Variables{});

    ASSERT_TRUE(!!window);
    assertContainer(*window, windowDefinition.name, 0, 0, windowWidth, windowHeight, 1);
    const auto& slide = window->children[0];
    assertContainer(slide, slideDefinition.name, 0, 0, windowWidth, contentHeight);
}

TEST(LayoutTests, horizontalTiles2)
{
    constexpr int windowWidth = 350;
    constexpr int tileWidth = 100;
    constexpr int tileHeight = 100;

    layout2::ContainerDefinition windowDefinition;
    windowDefinition.name = "window";
    windowDefinition.width.pixels = windowWidth;
    windowDefinition.height.upstream = layout2::Upstream::children;

    auto& tilesRow = windowDefinition.tileRow;
    tilesRow.resize(5);

    for(std::size_t i = 0; i < 5; ++i)
    {
        layout2::ContainerDefinition& tileDefinition = tilesRow[i];
        tileDefinition.width.pixels = 100;
        tileDefinition.height.pixels = 100;
    }

    services::LayoutSetupService setupService {};
    const auto setupList = setupService.buildSetupList(windowDefinition);

    services::LayoutService2 layoutService;
    const auto window = layoutService.calculate(setupList, objects::layout::Variables{});

    ASSERT_TRUE(!!window);
    assertContainer(*window, windowDefinition.name, 0, 0, windowWidth, tileHeight * 2, 5);

    const auto& tile1 = window->children[0];
    assertContainer(tile1, "", 0, 0, tileWidth, tileHeight);

    const auto& tile2 = window->children[1];
    assertContainer(tile2, "", 100, 0, tileWidth, tileHeight);

    const auto& tile3 = window->children[2];
    assertContainer(tile3, "", 200, 0, tileWidth, tileHeight);

    const auto& tile4 = window->children[3];
    assertContainer(tile4, "", 0, 100, tileWidth, tileHeight);

    const auto& tile5 = window->children[4];
    assertContainer(tile5, "", 100, 100, tileWidth, tileHeight);
}

TEST(LayoutTests, verticalTiles2)
{
    constexpr int windowHeight = 350;
    constexpr int tileWidth = 100;
    constexpr int tileHeight = 100;

    layout2::ContainerDefinition windowDefinition;
    windowDefinition.name = "window";
    windowDefinition.height.pixels = windowHeight;
    windowDefinition.width.upstream = layout2::Upstream::children;

    auto& tilesColumn = windowDefinition.tileColumn;
    tilesColumn.resize(5);

    for(std::size_t i = 0; i < 5; ++i)
    {
        layout2::ContainerDefinition& tileDefinition = tilesColumn[i];
        tileDefinition.width.pixels = 100;
        tileDefinition.height.pixels = 100;
    }

    services::LayoutSetupService setupService {};
    const auto setupList = setupService.buildSetupList(windowDefinition);

    services::LayoutService2 layoutService;
    const auto window = layoutService.calculate(setupList, objects::layout::Variables{});

    ASSERT_TRUE(!!window);
    assertContainer(*window, windowDefinition.name, 0, 0, tileWidth * 2, windowHeight, 5);

    const auto& tile1 = window->children[0];
    assertContainer(tile1, "", 0, 0, tileWidth, tileHeight);

    const auto& tile2 = window->children[1];
    assertContainer(tile2, "", 0, 100, tileWidth, tileHeight);

    const auto& tile3 = window->children[2];
    assertContainer(tile3, "", 0, 200, tileWidth, tileHeight);

    const auto& tile4 = window->children[3];
    assertContainer(tile4, "", 100, 0, tileWidth, tileHeight);

    const auto& tile5 = window->children[4];
    assertContainer(tile5, "", 100, 100, tileWidth, tileHeight);
}

//TODO: unit test for layout like word-wrap text: letters make words(lines of nodes) and words can jump to next line
//TODO: test three-column layout with a left column filled with tiles vertically and taking width from it's contents, then a flexible-width column and a fixed width column
//TODO: make a test of a 0-size container filled with tiles, which have to make a column of tiles or a row of tiles accordingly if they are horizontal or vertical alignment

//TODO: write a test for a 3 column layout with a flexible column in the middle, which contains another three columns with a flexible column in the middle,
//where one of the columns has width defined as a percent of the parents width