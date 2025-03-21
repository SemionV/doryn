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

using namespace layout::util2;
using namespace entities::layout::util2;

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
    const auto screenDefinition = def() | w(screenWidth) | h(screenHeight) | floating({
        definition
    });

    testLayout(screenDefinition, {
        con(screenDefinition.name) | _w(screenWidth) | _h(screenHeight) | kids({ 1 }),
        con(definition.name) | _x(x) | _y(y) | _w(width) | _h(height)
    });
}

TEST(LayoutTests, relativePosition)
{
    constexpr int _x = 30, _y = 10, width = 150, height = 100;
    testWindow(1024, 768, _x, _y, width, height, def("window") | x(_x) | y(_y) | w(width) | h(100));
}

TEST(LayoutTests, centeredPosition)
{
    constexpr int width = 150, height = 100;
    testWindow(350, 400, 100, 150, width, height, def("window") | x(al::center) | y(al::center) | w(width) | h(height));
}

TEST(LayoutTests, originPosition)
{
    constexpr int width = 150, height = 100;
    testWindow(350, 400, 0, 0, width, height, def("window") | w(width) | h(height));
}

TEST(LayoutTests, fullScreen)
{
    testWindow(1024, 768, 0, 0, 1024, 768, def("window") | w(us::parent) | h(us::parent));
}

TEST(LayoutTests, percentDimensions)
{
    testWindow(1000, 1000, 100, 200, 300, 100, def("window") | x(10.f) | y(20.f) | w(30.f) | h(10.f));
}

TEST(LayoutTests, rowOfTwoColumns)
{
    constexpr int windowWidth = 1024, windowHeight = 768, column1Width = 124;

    const auto definition = def("window") | w(windowWidth) | h(windowHeight) | columns({
        def("column1") | w(column1Width),
        def("column2") | w(us::fill)
    });

    testLayout(definition, {
        con(definition.name) | _w(windowWidth) | _h(windowHeight) | kids({ 1, 2 }),
        con("column1") | _x(0) | _w(column1Width) | _h(windowHeight),
        con("column2") | _x(column1Width) | _w(windowWidth - column1Width) | _h(windowHeight)
    });
}

TEST(LayoutTests, rowOfTwoColumnsWithFlexibleFirst)
{
    constexpr int windowWidth = 1024, windowHeight = 768, column1Width = 124;

    const auto definition = def("window") | w(windowWidth) | h(windowHeight) | columns({
        def("column1") | w(us::fill),
        def("column2") | w(column1Width)
    });

    testLayout(definition, {
        con(definition.name) | _w(windowWidth) | _h(windowHeight) | kids({ 1, 2 }),
        con("column1") | _x(0) | _w(windowWidth - column1Width) | _h(windowHeight),
        con("column2") | _x(windowWidth - column1Width) | _w(column1Width) | _h(windowHeight)
    });
}

TEST(LayoutTests, columnOfTwoRows)
{
    constexpr int windowWidth = 1024, windowHeight = 768, row1Height = 168;

    const auto definition = def("window") | w(windowWidth) | h(windowHeight) | rows({
        def("column1") | h(row1Height),
        def("column2") | h(us::fill)
    });

    testLayout(definition, {
        con(definition.name) | _w(windowWidth) | _h(windowHeight) | kids({ 1, 2 }),
        con("column1") | _y(0) | _w(windowWidth) | _h(row1Height),
        con("column2") | _y(row1Height) | _w(windowWidth) | _h(windowHeight - row1Height)
    });
}

TEST(LayoutTests, windowWidthStretchedByContent)
{
    constexpr int screenWidth = 1024, screenHeight = 768, windowHeight = 400, column1Width = 100, column2Width = 200;

    constexpr int expectedWindowWidth = column1Width + column2Width;
    const int expectedWindowX = static_cast<int>(std::round(static_cast<float>(screenWidth - expectedWindowWidth) / 2.f));
    const int expectedWindowY = static_cast<int>(std::round(static_cast<float>(screenHeight - windowHeight) / 2.f));

    const auto definition = def("screen") | w(screenWidth) | h(screenHeight) | floating({
        def("window") | x(al::center) | y(al::center) | w(us::children) | h(windowHeight) | columns({
            def("column1") | w(column1Width),
            def("column2") | w(column2Width)
        })
    });

    testLayout(definition, {
        con("screen") | _w(screenWidth) | _h(screenHeight) | kids({ 1 }),
        con("window") | _x(expectedWindowX) | _y(expectedWindowY) | _w(expectedWindowWidth) | _h(windowHeight) | kids({ 2, 3 }),
        con("column1") | _x(0) | _w(column1Width) | _h(windowHeight) | parent(1),
        con("column2") | _x(column1Width) | _w(column2Width) | _h(windowHeight) | parent(1)
    });
}

TEST(LayoutTests, windowHeightStretchedByContent)
{
    constexpr int screenWidth = 1024, screenHeight = 768, windowWidth = 400, row1Height = 100, row2Height = 200;

    const auto definition = def("screen") | w(screenWidth) | h(screenHeight) | floating({
        def("window") | x(al::center) | y(al::center) | w(windowWidth) | h(us::children) | rows({
            def("row1") | h(row1Height),
            def("row2") | h(row2Height)
        })
    });

    constexpr int expectedWindowHeight = row1Height + row2Height;
    const int expectedWindowX = static_cast<int>(std::round(static_cast<float>(screenWidth - windowWidth) / 2.f));
    const int expectedWindowY = static_cast<int>(std::round(static_cast<float>(screenHeight - expectedWindowHeight) / 2.f));

    testLayout(definition, {
        con("screen") | _w(screenWidth) | _h(screenHeight) | kids({ 1 }),
        con("window") | _x(expectedWindowX) | _y(expectedWindowY) | _w(windowWidth) | _h(expectedWindowHeight) | kids({ 2, 3 }),
        con("row1") | _y(0) | _w(windowWidth) | _h(row1Height) | parent(1),
        con("row2") | _y(row1Height) | _w(windowWidth) | _h(row2Height) | parent(1)
    });
}

TEST(LayoutTests, rowOfThreeColumns)
{
    constexpr int windowWidth = 1024, windowHeight = 768, column1Width = 124, column3Width = 50;

    const auto definition = def("window") | w(windowWidth) | h(windowHeight) | columns({
        def("column1") | w(column1Width),
        def("column2") | w(us::fill),
        def("column3") | w(column3Width)
    });

    testLayout(definition, {
        con("window") | _w(windowWidth) | _h(windowHeight) | kids({1, 2, 3}),
        con("column1") | _x(0) | _w(column1Width) | _h(windowHeight),
        con("column2") | _x(column1Width) | _w(windowWidth - column1Width - column3Width) | _h(windowHeight),
        con("column3") | _x(windowWidth - column3Width) | _w(column3Width) | _h(windowHeight),
    });
}

TEST(LayoutTests, columnOfThreeRows)
{
    constexpr int windowWidth = 1024, windowHeight = 768, row1Height = 168, row3Height = 50;

    const auto definition = def("window") | w(windowWidth) | h(windowHeight) | rows({
        def("row1") | h(row1Height),
        def("row2") | h(us::fill),
        def("row3") | h(row3Height)
    });

    testLayout(definition, {
        con("window") | _w(windowWidth) | _h(windowHeight) | kids({1, 2, 3}),
        con("row1") | _y(0) | _w(windowWidth) | _h(row1Height),
        con("row2") | _y(row1Height) | _w(windowWidth) | _h(windowHeight - row1Height - row3Height),
        con("row3") | _y(windowHeight - row3Height) | _w(windowWidth) | _h(row3Height),
    });
}

TEST(LayoutTests, combinedThreeColumnAndRowsGridLayout)
{
    constexpr int windowWidth = 1024, windowHeight = 768, column1Width = 124, column3Width = 50, row1Height = 168, row3Height = 50;

    const auto definition = def("window") | w(windowWidth) | h(windowHeight) | columns({
        def("column1") | w(column1Width),
        def("column2") | w(us::fill) | rows({
            def("row1") | h(row1Height),
            def("row2") | h(us::fill),
            def("row3") | h(row3Height)
        }),
        def("column3") | w(column3Width)
    });

    constexpr int expectedColumn2Width = windowWidth - column1Width - column3Width;

    testLayout(definition, {
        con("window") | _w(windowWidth) | _h(windowHeight) | kids({1, 2, 6}),
        con("column1") | _x(0) | _w(column1Width) | _h(windowHeight),
        con("column2") | _x(column1Width) | _w(windowWidth - column1Width - column3Width) | _h(windowHeight) | kids({ 3, 4, 5 }),
        con("row1") | _y(0) | _w(expectedColumn2Width) | _h(row1Height) | parent(2),
        con("row2") | _y(row1Height) | _w(expectedColumn2Width) | _h(windowHeight - row1Height - row3Height) | parent(2),
        con("row3") | _y(windowHeight - row3Height) | _w(expectedColumn2Width) | _h(row3Height) | parent(2),
        con("column3") | _x(windowWidth - column3Width) | _w(column3Width) | _h(windowHeight)
    });
}

TEST(LayoutTests, scrollableContent)
{
    constexpr int windowWidth = 400, windowHeight = 400, contentHeight = 3000;

    const auto definition = def("window") | w(windowWidth) | h(windowHeight) | slides({
        def("slide") | w(us::parent) | h(contentHeight)
    });

    testLayout(definition, {
        con("window") | _w(windowWidth) | _h(windowHeight) | kids({1}),
        con("slide") | _w(windowWidth) | _h(contentHeight)
    });
}

TEST(LayoutTests, horizontalTiles2)
{
    constexpr int windowWidth = 350, tileWidth = 100, tileHeight = 100;

    const auto definition = def("window") | w(windowWidth) | h(us::children) | rowTiles({
        def() | w(tileWidth) | h(tileHeight),
        def() | w(tileWidth) | h(tileHeight),
        def() | w(tileWidth) | h(tileHeight),
        def() | w(tileWidth) | h(tileHeight),
        def() | w(tileWidth) | h(tileHeight)
    });

    testLayout(definition, {
        con("window") | _w(windowWidth) | _h(tileHeight * 2) | kids({1, 2, 3, 4, 5}),
        con() | _x(0) | _y(0) | _w(tileWidth) | _h(tileHeight),
        con() | _x(100) | _y(0) | _w(tileWidth) | _h(tileHeight),
        con() | _x(200) | _y(0) | _w(tileWidth) | _h(tileHeight),
        con() | _x(0) | _y(100) | _w(tileWidth) | _h(tileHeight),
        con() | _x(100) | _y(100) | _w(tileWidth) | _h(tileHeight)
    });
}

TEST(LayoutTests, verticalTiles2)
{
    constexpr int windowHeight = 350, tileWidth = 100, tileHeight = 100;

    const auto definition = def("window") | w(us::children) | h(windowHeight) | columnTiles({
        def() | w(tileWidth) | h(tileHeight),
        def() | w(tileWidth) | h(tileHeight),
        def() | w(tileWidth) | h(tileHeight),
        def() | w(tileWidth) | h(tileHeight),
        def() | w(tileWidth) | h(tileHeight)
    });

    testLayout(definition, {
        con("window") | _w(tileWidth * 2) | _h(windowHeight) | kids({1, 2, 3, 4, 5}),
        con() | _x(0) | _y(0) | _w(tileWidth) | _h(tileHeight),
        con() | _x(0) | _y(100) | _w(tileWidth) | _h(tileHeight),
        con() | _x(0) | _y(200) | _w(tileWidth) | _h(tileHeight),
        con() | _x(100) | _y(0) | _w(tileWidth) | _h(tileHeight),
        con() | _x(100) | _y(100) | _w(tileWidth) | _h(tileHeight)
    });
}

//3 column layout with a flexible column in the middle, which contains another three columns with a flexible column in the middle,
//where one of the columns has width defined as a percent of the parents width
TEST(LayoutTests, threeColumnsWithThreeColumns)
{
    constexpr int windowWidth = 1024, windowHeight = 768, column1Width = 124, column3Width = 50, column1_3Width = 100;
    constexpr float column1_1Width = 50.f;

    const auto definition = def("window") | w(windowWidth) | h(windowHeight) | columns({
        def("column1") | w(column1Width),
        def("column2") | w(us::fill) | columns({
            def("column1_1") | w(column1_1Width),
            def("column1_2") | w(us::fill),
            def("column1_3") | w(column1_3Width)
        }),
        def("column3") | w(column3Width)
    });

    constexpr int column2WidthExpected = windowWidth - column1Width - column3Width;
    constexpr int column1_1WidthExpected = column2WidthExpected * 0.01f * column1_1Width;
    constexpr int column1_2WidthExpected = column2WidthExpected - column1_1WidthExpected - column1_3Width;

    testLayout(definition, {
        con("column2") | _x(column1Width) | _w(column2WidthExpected) | _h(windowHeight) | kids({ 1, 2, 3}),
            con("column1_1") | _x(0) | _w(column1_1WidthExpected) | _h(windowHeight),
            con("column1_2") | _x(column1_1WidthExpected) | _w(column1_2WidthExpected) | _h(windowHeight),
            con("column1_3") | _x(column1_1WidthExpected + column1_2WidthExpected) | _w(column1_3Width) | _h(windowHeight),
    }, 2);
}

//Checks that root container gets its size calculated from its child with defined size and that a popup is positioned correctly after this
TEST(LayoutTests, stratchedContainerWithPopup)
{
    constexpr int slideWidth = 1024, slideHeight = 768, popupWidth = 100, popupHeight = 100;

    const auto definition = def("window") | w(us::children) | h(us::children) | slides({
        def("slide") | w(slideWidth) | h(slideHeight)
    }) | floating({
        def("popup") | x(al::center) | y(al::center) | w(popupWidth) | h(popupHeight)
    });

    const int expectedPopupX = static_cast<int>(std::round(static_cast<float>(slideWidth - popupWidth) / 2.f));
    const int expectedPopupY = static_cast<int>(std::round(static_cast<float>(slideHeight - popupHeight) / 2.f));

    testLayout(definition, {
        con("window") | _w(slideWidth) | _h(slideHeight) | kids({1, 2}),
        con("slide") | _w(slideWidth) | _h(slideHeight),
        con("popup") | _x(expectedPopupX) | _y(expectedPopupY) | _w(popupWidth) | _h(popupHeight),
    });
}

//TODO: unit test for layout like word-wrap text: letters make words(lines of nodes) and words can jump to next line
//TODO: test three-column layout with a left column filled with tiles vertically and taking width from it's contents, then a flexible-width column and a fixed width column
//TODO: make a test of a 0-size container filled with tiles, which have to make a column of tiles or a row of tiles accordingly if they are horizontal or vertical alignment