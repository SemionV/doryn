#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <dory/core/resources/objects/layout.h>
#include <dory/core/services/layoutService.h>
#include <dory/core/services/layoutSetupService.h>
#include <dory/core/repositories/iLayoutRepository.h>
#include <dory/core/registry.h>
#include <spdlog/fmt/fmt.h>

#include "mocks/entityRepository.h"

using namespace dory;
using namespace dory::core;
using namespace dory::core::resources;
using namespace dory::core::resources::entities::layout;

using namespace objects::layout::util;
using namespace entities::layout::util;

class LayoutRepository: public EntityRepositoryMock<repositories::ILayoutRepository>
{};

void assertContainer(const Container& container, const Name& name, const std::size_t parentIndex, const int x, const int y,
    const int width, const int height)
{
    EXPECT_EQ(container.name, name);
    EXPECT_EQ(container.parent, parentIndex) << name;
    EXPECT_EQ(container.size.width, width) << name;
    EXPECT_EQ(container.size.height, height) << name;
    EXPECT_EQ(container.position.x, x) << name;
    EXPECT_EQ(container.position.y, y) << name;
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

void testLayout(const objects::layout::ContainerDefinition& definition, const std::vector<Container>& expected, const std::size_t offsetIndex = 0)
{
    services::LayoutSetupService setupService {};
    const auto setupList = setupService.buildSetupList(definition);

    Layout layout {};
    services::LayoutService layoutService;
    layoutService.buildLayout(setupList, objects::layout::Variables{}, layout);

    assertContainerTree(layout, offsetIndex, expected);
}

void testWindow(const int screenWidth, const int screenHeight, const int x, const int y, const int width, const int height,
    const objects::layout::ContainerDefinition& definition)
{
    const auto screenDefinition = def() | w(screenWidth) | h(screenHeight) | floating({
        definition
    });

    testLayout(screenDefinition, {
        con(containers::hash::hash(screenDefinition.name)) | _w(screenWidth) | _h(screenHeight) | kids({ 1 }),
        con(containers::hash::hash(definition.name)) | _x(x) | _y(y) | _w(width) | _h(height)
    });
}

TEST(LayoutTests, relativePosition)
{
    constexpr int _x = 30, _y = 10, width = 150, height = 100;
    testWindow(1024, 768, _x, _y, width, height, def("window"_id) | x(_x) | y(_y) | w(width) | h(100));
}

TEST(LayoutTests, centeredPosition)
{
    constexpr int width = 150, height = 100;
    testWindow(350, 400, 100, 150, width, height, def("window"_id) | x(al::center) | y(al::center) | w(width) | h(height));
}

TEST(LayoutTests, originPosition)
{
    constexpr int width = 150, height = 100;
    testWindow(350, 400, 0, 0, width, height, def("window"_id) | w(width) | h(height));
}

TEST(LayoutTests, fullScreen)
{
    testWindow(1024, 768, 0, 0, 1024, 768, def("window"_id) | w(us::parent) | h(us::parent));
}

TEST(LayoutTests, percentDimensions)
{
    testWindow(1000, 1000, 100, 200, 300, 100, def("window"_id) | x(10.f) | y(20.f) | w(30.f) | h(10.f));
}

TEST(LayoutTests, rowOfTwoColumns)
{
    constexpr int windowWidth = 1024, windowHeight = 768, column1Width = 124;

    const auto definition = def("window"_id) | w(windowWidth) | h(windowHeight) | columns({
        def("column1"_id) | w(column1Width),
        def("column2"_id) | w(us::fill)
    });

    testLayout(definition, {
        con(containers::hash::hash(definition.name)) | _w(windowWidth) | _h(windowHeight) | kids({ 1, 2 }),
        con("column1"_id) | _x(0) | _w(column1Width) | _h(windowHeight),
        con("column2"_id) | _x(column1Width) | _w(windowWidth - column1Width) | _h(windowHeight)
    });
}

TEST(LayoutTests, rowOfTwoColumnsWithFlexibleFirst)
{
    constexpr int windowWidth = 1024, windowHeight = 768, column1Width = 124;

    const auto definition = def("window"_id) | w(windowWidth) | h(windowHeight) | columns({
        def("column1"_id) | w(us::fill),
        def("column2"_id) | w(column1Width)
    });

    testLayout(definition, {
        con(containers::hash::hash(definition.name)) | _w(windowWidth) | _h(windowHeight) | kids({ 1, 2 }),
        con("column1"_id) | _x(0) | _w(windowWidth - column1Width) | _h(windowHeight),
        con("column2"_id) | _x(windowWidth - column1Width) | _w(column1Width) | _h(windowHeight)
    });
}

TEST(LayoutTests, columnOfTwoRows)
{
    constexpr int windowWidth = 1024, windowHeight = 768, row1Height = 168;

    const auto definition = def("window"_id) | w(windowWidth) | h(windowHeight) | rows({
        def("column1"_id) | h(row1Height),
        def("column2"_id) | h(us::fill)
    });

    testLayout(definition, {
        con(containers::hash::hash(definition.name)) | _w(windowWidth) | _h(windowHeight) | kids({ 1, 2 }),
        con("column1"_id) | _y(0) | _w(windowWidth) | _h(row1Height),
        con("column2"_id) | _y(row1Height) | _w(windowWidth) | _h(windowHeight - row1Height)
    });
}

TEST(LayoutTests, windowWidthStretchedByContent)
{
    constexpr int screenWidth = 1024, screenHeight = 768, windowHeight = 400, column1Width = 100, column2Width = 200;

    constexpr int expectedWindowWidth = column1Width + column2Width;
    const int expectedWindowX = static_cast<int>(std::round(static_cast<float>(screenWidth - expectedWindowWidth) / 2.f));
    const int expectedWindowY = static_cast<int>(std::round(static_cast<float>(screenHeight - windowHeight) / 2.f));

    const auto definition = def("screen"_id) | w(screenWidth) | h(screenHeight) | floating({
        def("window"_id) | x(al::center) | y(al::center) | w(us::children) | h(windowHeight) | columns({
            def("column1"_id) | w(column1Width),
            def("column2"_id) | w(column2Width)
        })
    });

    testLayout(definition, {
        con("screen"_id) | _w(screenWidth) | _h(screenHeight) | kids({ 1 }),
        con("window"_id) | _x(expectedWindowX) | _y(expectedWindowY) | _w(expectedWindowWidth) | _h(windowHeight) | kids({ 2, 3 }),
        con("column1"_id) | _x(0) | _w(column1Width) | _h(windowHeight) | parent(1),
        con("column2"_id) | _x(column1Width) | _w(column2Width) | _h(windowHeight) | parent(1)
    });
}

TEST(LayoutTests, windowHeightStretchedByContent)
{
    constexpr int screenWidth = 1024, screenHeight = 768, windowWidth = 400, row1Height = 100, row2Height = 200;

    const auto definition = def("screen"_id) | w(screenWidth) | h(screenHeight) | floating({
        def("window"_id) | x(al::center) | y(al::center) | w(windowWidth) | h(us::children) | rows({
            def("row1"_id) | h(row1Height),
            def("row2"_id) | h(row2Height)
        })
    });

    constexpr int expectedWindowHeight = row1Height + row2Height;
    const int expectedWindowX = static_cast<int>(std::round(static_cast<float>(screenWidth - windowWidth) / 2.f));
    const int expectedWindowY = static_cast<int>(std::round(static_cast<float>(screenHeight - expectedWindowHeight) / 2.f));

    testLayout(definition, {
        con("screen"_id) | _w(screenWidth) | _h(screenHeight) | kids({ 1 }),
        con("window"_id) | _x(expectedWindowX) | _y(expectedWindowY) | _w(windowWidth) | _h(expectedWindowHeight) | kids({ 2, 3 }),
        con("row1"_id) | _y(0) | _w(windowWidth) | _h(row1Height) | parent(1),
        con("row2"_id) | _y(row1Height) | _w(windowWidth) | _h(row2Height) | parent(1)
    });
}

TEST(LayoutTests, rowOfThreeColumns)
{
    constexpr int windowWidth = 1024, windowHeight = 768, column1Width = 124, column3Width = 50;

    const auto definition = def("window"_id) | w(windowWidth) | h(windowHeight) | columns({
        def("column1"_id) | w(column1Width),
        def("column2"_id) | w(us::fill),
        def("column3"_id) | w(column3Width)
    });

    testLayout(definition, {
        con("window"_id) | _w(windowWidth) | _h(windowHeight) | kids({1, 2, 3}),
        con("column1"_id) | _x(0) | _w(column1Width) | _h(windowHeight),
        con("column2"_id) | _x(column1Width) | _w(windowWidth - column1Width - column3Width) | _h(windowHeight),
        con("column3"_id) | _x(windowWidth - column3Width) | _w(column3Width) | _h(windowHeight),
    });
}

TEST(LayoutTests, columnOfThreeRows)
{
    constexpr int windowWidth = 1024, windowHeight = 768, row1Height = 168, row3Height = 50;

    const auto definition = def("window"_id) | w(windowWidth) | h(windowHeight) | rows({
        def("row1"_id) | h(row1Height),
        def("row2"_id) | h(us::fill),
        def("row3"_id) | h(row3Height)
    });

    testLayout(definition, {
        con("window"_id) | _w(windowWidth) | _h(windowHeight) | kids({1, 2, 3}),
        con("row1"_id) | _y(0) | _w(windowWidth) | _h(row1Height),
        con("row2"_id) | _y(row1Height) | _w(windowWidth) | _h(windowHeight - row1Height - row3Height),
        con("row3"_id) | _y(windowHeight - row3Height) | _w(windowWidth) | _h(row3Height),
    });
}

TEST(LayoutTests, combinedThreeColumnAndRowsGridLayout)
{
    constexpr int windowWidth = 1024, windowHeight = 768, column1Width = 124, column3Width = 50, row1Height = 168, row3Height = 50;

    const auto definition = def("window"_id) | w(windowWidth) | h(windowHeight) | columns({
        def("column1"_id) | w(column1Width),
        def("column2"_id) | w(us::fill) | rows({
            def("row1"_id) | h(row1Height),
            def("row2"_id) | h(us::fill),
            def("row3"_id) | h(row3Height)
        }),
        def("column3"_id) | w(column3Width)
    });

    constexpr int expectedColumn2Width = windowWidth - column1Width - column3Width;

    testLayout(definition, {
        con("window"_id) | _w(windowWidth) | _h(windowHeight) | kids({1, 2, 6}),
        con("column1"_id) | _x(0) | _w(column1Width) | _h(windowHeight),
        con("column2"_id) | _x(column1Width) | _w(windowWidth - column1Width - column3Width) | _h(windowHeight) | kids({ 3, 4, 5 }),
        con("row1"_id) | _y(0) | _w(expectedColumn2Width) | _h(row1Height) | parent(2),
        con("row2"_id) | _y(row1Height) | _w(expectedColumn2Width) | _h(windowHeight - row1Height - row3Height) | parent(2),
        con("row3"_id) | _y(windowHeight - row3Height) | _w(expectedColumn2Width) | _h(row3Height) | parent(2),
        con("column3"_id) | _x(windowWidth - column3Width) | _w(column3Width) | _h(windowHeight)
    });
}

TEST(LayoutTests, scrollableContent)
{
    constexpr int windowWidth = 400, windowHeight = 400, contentHeight = 3000;

    const auto definition = def("window"_id) | w(windowWidth) | h(windowHeight) | slides({
        def("slide"_id) | w(us::parent) | h(contentHeight)
    });

    testLayout(definition, {
        con("window"_id) | _w(windowWidth) | _h(windowHeight) | kids({1}),
        con("slide"_id) | _w(windowWidth) | _h(contentHeight)
    });
}

TEST(LayoutTests, horizontalTiles2)
{
    constexpr int windowWidth = 350, tileWidth = 100, tileHeight = 100;

    const auto definition = def("window"_id) | w(windowWidth) | h(us::children) | rowTiles({
        def() | w(tileWidth) | h(tileHeight),
        def() | w(tileWidth) | h(tileHeight),
        def() | w(tileWidth) | h(tileHeight),
        def() | w(tileWidth) | h(tileHeight),
        def() | w(tileWidth) | h(tileHeight)
    });

    testLayout(definition, {
        con("window"_id) | _w(windowWidth) | _h(tileHeight * 2) | kids({1, 2, 3, 4, 5}),
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

    const auto definition = def("window"_id) | w(us::children) | h(windowHeight) | columnTiles({
        def() | w(tileWidth) | h(tileHeight),
        def() | w(tileWidth) | h(tileHeight),
        def() | w(tileWidth) | h(tileHeight),
        def() | w(tileWidth) | h(tileHeight),
        def() | w(tileWidth) | h(tileHeight)
    });

    testLayout(definition, {
        con("window"_id) | _w(tileWidth * 2) | _h(windowHeight) | kids({1, 2, 3, 4, 5}),
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

    const auto definition = def("window"_id) | w(windowWidth) | h(windowHeight) | columns({
        def("column1"_id) | w(column1Width),
        def("column2"_id) | w(us::fill) | columns({
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

//Fill a 0-sized container with tiles and expect to get a column of tiles one below another
TEST(LayoutTests, tilesStack)
{
    constexpr int tileWidth = 100, tileHeight = 100;

    const auto definition = def("window") | w(us::children) | h(us::children) | rowTiles({
        def() | w(tileWidth) | h(tileHeight),
        def() | w(tileWidth) | h(tileHeight),
        def() | w(tileWidth) | h(tileHeight),
        def() | w(tileWidth) | h(tileHeight),
        def() | w(tileWidth) | h(tileHeight)
    });

    testLayout(definition, {
        con("window") | _w(tileWidth) | _h(tileHeight * 5) | kids({1, 2, 3, 4, 5}),
        con() | _x(0) | _y(0) | _w(tileWidth) | _h(tileHeight),
        con() | _x(0) | _y(100) | _w(tileWidth) | _h(tileHeight),
        con() | _x(0) | _y(200) | _w(tileWidth) | _h(tileHeight),
        con() | _x(0) | _y(300) | _w(tileWidth) | _h(tileHeight),
        con() | _x(0) | _y(400) | _w(tileWidth) | _h(tileHeight)
    });
}

//Fill a 0-sized container with tiles and expect to get a line of tiles one after another
TEST(LayoutTests, tilesLine)
{
    constexpr int tileWidth = 100, tileHeight = 100;

    const auto definition = def("window") | w(us::children) | h(us::children) | columnTiles({
        def() | w(tileWidth) | h(tileHeight),
        def() | w(tileWidth) | h(tileHeight),
        def() | w(tileWidth) | h(tileHeight),
        def() | w(tileWidth) | h(tileHeight),
        def() | w(tileWidth) | h(tileHeight)
    });

    testLayout(definition, {
        con("window") | _w(tileWidth * 5) | _h(tileHeight) | kids({1, 2, 3, 4, 5}),
        con() | _x(0) | _y(0) | _w(tileWidth) | _h(tileHeight),
        con() | _x(100) | _y(0) | _w(tileWidth) | _h(tileHeight),
        con() | _x(200) | _y(0) | _w(tileWidth) | _h(tileHeight),
        con() | _x(300) | _y(0) | _w(tileWidth) | _h(tileHeight),
        con() | _x(400) | _y(0) | _w(tileWidth) | _h(tileHeight)
    });
}

//layout like line-wrap text: letters make words(lines of nodes) and words can jump to next line
TEST(LayoutTests, textLayout)
{
    constexpr std::size_t wordsCount = 5, lettersPerWord = 2;
    constexpr int windowWidth = 20, wordHeight = 10, letterWidth = 5, wordWidth = letterWidth * lettersPerWord;
    const int wordsPerLine = std::floor(windowWidth / wordWidth);
    const int linesCount = std::ceil(static_cast<float>(wordsCount) / static_cast<float>(wordsPerLine));

    const auto letters = std::vector{ def() | w(letterWidth), def() | w(letterWidth) };

    auto definition = def("window") | w(windowWidth) | h(us::children);

    std::vector<objects::layout::ContainerDefinition> wordDefinitions;
    wordDefinitions.reserve(wordsCount);
    for(std::size_t i = 0; i < wordsCount; ++i)
    {
        auto& wordDefinition = wordDefinitions.emplace_back(def(fmt::format("word-{}", i + 1)) | w(us::children) | h(wordHeight));
        std::vector<objects::layout::ContainerDefinition> letterDefinitions;
        letterDefinitions.reserve(lettersPerWord);
        for(std::size_t j = 0; j < lettersPerWord; ++j)
        {
            letterDefinitions.emplace_back(def(fmt::format("letter-{}-{}", i + 1, j + 1)) | w(letterWidth));
        }
        wordDefinition.columns = letterDefinitions;
    }
    definition.tileRow = wordDefinitions;

    std::vector<Container> containers;
    containers.reserve(1 + wordsCount + wordsCount * lettersPerWord);
    auto& windowContainer = containers.emplace_back(con("window") | _w(windowWidth) | _h(linesCount * wordHeight));
    for(std::size_t i = 0; i < wordsCount; ++i)
    {
        std::size_t wordIndex = 1 + i * (lettersPerWord + 1);//arithmetic progression
        const int lineNumber = std::floor(i / wordsPerLine);
        const int expectedWordX = static_cast<int>(i % wordsPerLine) * wordWidth;
        auto& wordContainer = containers.emplace_back(con(fmt::format("word-{}", i + 1)) | _x(expectedWordX) | _y(lineNumber * wordHeight) | _w(wordWidth) | _h(wordHeight));
        for(std::size_t j = 0; j < lettersPerWord; ++j)
        {
            std::size_t letterIndex = j + 1;
            const int expectedLetterX = static_cast<int>(j) * letterWidth;
            containers.emplace_back(con(fmt::format("letter-{}-{}", i + 1, letterIndex)) | _x(expectedLetterX) | _w(letterWidth) | _h(wordHeight) | parent(wordIndex));
            wordContainer.children.emplace_back(wordIndex + letterIndex);
        }

        windowContainer.children.emplace_back(wordIndex);
    }

    testLayout(definition, containers);
}

//three-column layout with a left column filled with tiles vertically and taking width from its contents, then a flexible-width column and a fixed width column
TEST(LayoutTests, expandedColumnWithContent)
{
    constexpr int windowWidth = 1024, windowHeight = 768, tileWidth = 100, tileHeight = 300, column3Width = 50;

    const auto definition = def("window") | w(windowWidth) | h(windowHeight) | columns({
        def("column1") | w(us::children) | columnTiles({
            def() | w(tileWidth) | h(tileHeight),
            def() | w(tileWidth) | h(tileHeight),
            def() | w(tileWidth) | h(tileHeight)
        }),
        def("column2") | w(us::fill),
        def("column3") | w(column3Width)
    });

    constexpr int column1WidthExpected = tileWidth * 2;
    testLayout(definition, {
        con("window") | _w(windowWidth) | _h(windowHeight) | kids({1, 5, 6}),
        con("column1") | _x(0) | _w(column1WidthExpected) | _h(windowHeight) | kids({2,3,4}),
            con() | _x(0) | _y(0) | _w(tileWidth) | _h(tileHeight) | parent(1),
            con() | _x(0) | _y(tileHeight) | _w(tileWidth) | _h(tileHeight) | parent(1),
            con() | _x(tileWidth) | _y(0) | _w(tileWidth) | _h(tileHeight) | parent(1),
        con("column2") | _x(column1WidthExpected) | _w(windowWidth - column1WidthExpected - column3Width) | _h(windowHeight),
        con("column3") | _x(windowWidth - column3Width) | _w(column3Width) | _h(windowHeight)
    });
}

//layout with complex hierarchy(see https://miro.com/app/board/uXjVMhToss4=/?moveToWidget=3458764618857736489&cot=14)
TEST(LayoutTests, completeLayout)
{
    constexpr int windowWidth = 1024, windowHeight = 768, column3Width = 50, column1_1Width = 100, view1Width = 100, view1Height = 100;
    constexpr int tileWidth = 50, tileHeight = 50;
    constexpr int popup1Width = 10, popup1Height = 10;
    constexpr int row2_1Height = 100;
    constexpr float column3_1Width = 50.f;

    constexpr int column1WidthExpected = column1_1Width + view1Width;
    constexpr int column2WidthExpected = windowWidth - column1WidthExpected - column3Width;
    const int tile2Width = static_cast<int>(std::floor(static_cast<float>(column2WidthExpected) / 2)), tile2Height = 10;
    constexpr int row2_3HeightExpected = tile2Height * 2;
    constexpr int row2_2HeightExpected = windowHeight - row2_1Height - row2_3HeightExpected;
    const int popup1XExpected = static_cast<int>(std::round((column3Width - popup1Width) / 2));
    const int popup1YExpected = static_cast<int>(std::round((windowHeight - popup1Height) / 2));
    const int column3_1WidthExpected = static_cast<int>(std::round(popup1Width * 0.01f * column3_1Width));
    const int column3_2WidthExpected = popup1Width - column3_1WidthExpected;

    const auto definition = def("window") | w(windowWidth) | h(windowHeight) | columns({
        def("column1") | w(us::children) | columns({
            def("column1_1") | w(column1_1Width) | rowTiles({
                def("tile_1_1_1") | w(tileWidth) | h(tileHeight),
                def("tile_1_1_2") | w(tileWidth) | h(tileHeight),
                def("tile_1_1_3") | w(tileWidth) | h(tileHeight),
                def("tile_1_1_4") | w(tileWidth) | h(tileHeight),
                def("tile_1_1_5") | w(tileWidth) | h(tileHeight)
            }),
            def("column1_2") | w(us::children) | slides({
                def("view1") | w(view1Width) | h(view1Height)
            })
        }),
        def("column2") | w(us::fill) | rows({
            def("row2_1") | h(row2_1Height),
            def("row2_2") | h(us::fill),
            def("row2_3") | h(us::children) | rowTiles({
                def("tile_2_1_1") | w(tile2Width) | h(tile2Height),
                def("tile_2_1_2") | w(tile2Width) | h(tile2Height),
                def("tile_2_1_3") | w(tile2Width) | h(tile2Height)
            })
        }),
        def("column3") | w(column3Width) | floating({
            def("popup1") | x(al::center) | y(al::center) | w(popup1Width) | h(popup1Height) | columns({
                def("column3_1") | w(50.f),
                def("column3_2") | w(us::fill)
            })
        })
    });

    testLayout(definition, {
        con("window") | _w(windowWidth) | _h(windowHeight) | kids({1, 10, 17}),                                                                                 //0
            con("column1") | _w(column1WidthExpected) | _h(windowHeight) | kids({2,8}),                                                                         //1
                con("column1_1") | _w(column1_1Width) | _h(windowHeight) | kids({3, 4, 5, 6, 7}) | parent(1),                                                   //2
                    con("tile_1_1_1") | _x(0) | _y(0) | _w(tileWidth) | _h(tileHeight) | parent(2),                                                             //3
                    con("tile_1_1_2") | _x(tileWidth) | _y(0) | _w(tileWidth) | _h(tileHeight) | parent(2),                                                     //4
                    con("tile_1_1_3") | _x(0) | _y(tileHeight) | _w(tileWidth) | _h(tileHeight) | parent(2),                                                    //5
                    con("tile_1_1_4") | _x(tileWidth) | _y(tileHeight) | _w(tileWidth) | _h(tileHeight) | parent(2),                                            //6
                    con("tile_1_1_5") | _x(0) | _y(tileHeight * 2) | _w(tileWidth) | _h(tileHeight) | parent(2),                                                //7
                con("column1_2") | _x(column1_1Width) | _w(view1Width) | _h(windowHeight) | kids({9}) | parent(1),                                              //8
                    con("view1") | _w(view1Width) | _h(view1Height) | parent(8),                                                                                //9
            con("column2") | _x(column1WidthExpected) | _w(column2WidthExpected) | _h(windowHeight) | kids({11,12,13}),                                         //10
                con("row2_1") | _y(0) | _w(column2WidthExpected) | _h(row2_1Height) | parent(10),                                                               //11
                con("row2_2") | _y(row2_1Height) | _w(column2WidthExpected) | _h(row2_2HeightExpected) | parent(10),                                            //12
                con("row2_3") | _y(windowHeight - row2_3HeightExpected) | _w(column2WidthExpected) | _h(row2_3HeightExpected) | kids({14,15,16}) | parent(10),  //13
                    con("tile_2_1_1") | _x(0) | _y(0) | _w(tile2Width) | _h(tile2Height) | parent(13),                                                          //14
                    con("tile_2_1_2") | _x(tile2Width) | _y(0) | _w(tile2Width) | _h(tile2Height) | parent(13),                                                 //15
                    con("tile_2_1_3") | _x(0) | _y(tile2Height) | _w(tile2Width) | _h(tile2Height) | parent(13),                                                //16
            con("column3") | _x(windowWidth - column3Width) | _w(column3Width) | _h(windowHeight) | kids({18}),                                                 //17
                con("popup1") | _x(popup1XExpected) | _y(popup1YExpected) | _w(popup1Width) | _h(popup1Height) | kids({19,20}) | parent(17),                    //18
                    con("column3_1") | _x(0) | _w(column3_1WidthExpected) | _h(popup1Height) | parent(18),                                                      //19
                    con("column3_2") | _x(column3_1WidthExpected) | _w(column3_2WidthExpected) | _h(popup1Height) | parent(18)                                  //20
    });
}