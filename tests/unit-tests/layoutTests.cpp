#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <dory/core/resources/scene/configuration.h>
#include <dory/core/resources/objects/layout.h>
#include <dory/core/services/layoutService.h>
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
    layout::ContainerDefinition root {};
    root.name = "root";

    root.position = layout::Position {
        layout::Dimension {},
        layout::Dimension {}
    };
    root.size = layout::Size {
        layout::Dimension { 150 },
        layout::Dimension { 100 }
    };

    services::LayoutService layoutService;
    const objects::layout::Container container = layoutService.calculate(root, objects::layout::Size{ 350, 400 });

    EXPECT_EQ(container.name, root.name);
    EXPECT_EQ(container.size.width, 150);
    EXPECT_EQ(container.size.height, 100);
    EXPECT_EQ(container.position.x, 0);
    EXPECT_EQ(container.position.y, 0);
}

TEST(LayoutTests, fullScreen)
{
    layout::ContainerDefinition root {};
    root.name = "root";

    services::LayoutService layoutService;
    constexpr objects::layout::Size availableSpace{ 1024, 768 };
    const objects::layout::Container container = layoutService.calculate(root, availableSpace);

    EXPECT_EQ(container.name, root.name);
    EXPECT_EQ(container.size.width, availableSpace.width);
    EXPECT_EQ(container.size.height, availableSpace.height);
    EXPECT_EQ(container.position.x, 0);
    EXPECT_EQ(container.position.y, 0);
}

TEST(LayoutTests, percentDimensions)
{
    layout::ContainerDefinition root {};
    root.name = "root";
    root.size = layout::Size {
        layout::Dimension { {}, {10.f} },
        layout::Dimension { {}, {20.f} }
    };
    root.position = layout::Position {
        layout::Dimension { {}, {30.f} },
        layout::Dimension { {}, {10.f} }
    };

    services::LayoutService layoutService;
    constexpr objects::layout::Size availableSpace{ 1000, 1000 };
    const objects::layout::Container container = layoutService.calculate(root, availableSpace);

    EXPECT_EQ(container.name, root.name);
    EXPECT_EQ(container.size.width, 100);
    EXPECT_EQ(container.size.height, 200);
    EXPECT_EQ(container.position.x, 300);
    EXPECT_EQ(container.position.y, 100);
}


TEST(LayoutTests, gridTwoColumnsLine)
{
    layout::ContainerDefinition root {};
    root.name = "root";
    root.horizontal = std::vector<layout::ContainerDefinition>{};

    root.horizontal->reserve(2);

    layout::ContainerDefinition& column1 = root.horizontal->emplace_back();
    column1.size = layout::Size { layout::Dimension{ 124 }, {} }; //124px width, 100% height

    layout::ContainerDefinition& column2 = root.horizontal->emplace_back(); //100% - 124px width, 100% height

    services::LayoutService layoutService;
    constexpr objects::layout::Size availableSpace{ 1024, 768 };
    const objects::layout::Container container = layoutService.calculate(root, availableSpace);

    EXPECT_EQ(container.name, root.name);
    EXPECT_EQ(container.children.size(), 2);

    assertContainer(container.children[0], column1.name, 0, 0, 124, availableSpace.height);
    assertContainer(container.children[1], column2.name, 124, 0, 900, availableSpace.height);
}

TEST(LayoutTests, gridTwoRowsLine)
{
    layout::ContainerDefinition root {};
    root.name = "root";
    root.vertical = std::vector<layout::ContainerDefinition>{};

    root.vertical->reserve(2);

    layout::ContainerDefinition& row1 = root.vertical->emplace_back();
    row1.size = layout::Size { {}, layout::Dimension{ 168 } };

    layout::ContainerDefinition& row2 = root.vertical->emplace_back();

    services::LayoutService layoutService;
    constexpr objects::layout::Size availableSpace{ 1024, 768 };
    const objects::layout::Container container = layoutService.calculate(root, availableSpace);

    EXPECT_EQ(container.name, root.name);
    EXPECT_EQ(container.children.size(), 2);

    assertContainer(container.children[0], row1.name, 0, 0, availableSpace.width, 168);
    assertContainer(container.children[1], row2.name, 0, 168, availableSpace.width, 600);
}

TEST(LayoutTests, gridThreeColumnsLine)
{
    layout::ContainerDefinition root {};
    root.name = "root";
    root.horizontal = std::vector<layout::ContainerDefinition>{};

    root.horizontal->reserve(3);

    layout::ContainerDefinition& column1 = root.horizontal->emplace_back();
    column1.size = layout::Size { layout::Dimension{ 124 }, {} };

    layout::ContainerDefinition& column2 = root.horizontal->emplace_back();

    layout::ContainerDefinition& column3 = root.horizontal->emplace_back();
    column3.size = layout::Size { layout::Dimension{ 100 }, {} };

    services::LayoutService layoutService;
    constexpr objects::layout::Size availableSpace{ 1024, 768 };
    const objects::layout::Container container = layoutService.calculate(root, availableSpace);

    EXPECT_EQ(container.name, root.name);
    EXPECT_EQ(container.children.size(), 3);

    assertContainer(container.children[0], column1.name, 0, 0, 124, availableSpace.height);
    assertContainer(container.children[1], column2.name, 124, 0, 800, availableSpace.height);
    assertContainer(container.children[2], column3.name, 924, 0, 100, availableSpace.height);
}

TEST(LayoutTests, gridThreeRowsLine)
{
    layout::ContainerDefinition root {};
    root.name = "root";
    root.vertical = std::vector<layout::ContainerDefinition>{};

    root.vertical->reserve(3);

    layout::ContainerDefinition& row1 = root.vertical->emplace_back();
    row1.size = layout::Size { {}, layout::Dimension{ 168 } };

    layout::ContainerDefinition& row2 = root.vertical->emplace_back();

    layout::ContainerDefinition& row3 = root.vertical->emplace_back();
    row3.size = layout::Size { {}, layout::Dimension{ 100 } };

    services::LayoutService layoutService;
    constexpr objects::layout::Size availableSpace{ 1024, 768 };
    const objects::layout::Container container = layoutService.calculate(root, availableSpace);

    EXPECT_EQ(container.name, root.name);
    EXPECT_EQ(container.children.size(), 3);

    assertContainer(container.children[0], row1.name, 0, 0, availableSpace.width, 168);
    assertContainer(container.children[1], row2.name, 0, 168, availableSpace.width, 500);
    assertContainer(container.children[2], row3.name, 0, 668, availableSpace.width, 100);
}

TEST(LayoutTests, combinedThreeColumnAndRowsGridLayout)
{
    layout::ContainerDefinition root {};
    root.name = "root";
    root.horizontal = std::vector<layout::ContainerDefinition>(3);

    layout::ContainerDefinition& column1 = (*root.horizontal)[0];
    column1.size = layout::Size { layout::Dimension{ 124 }, {} };

    layout::ContainerDefinition& column2 = (*root.horizontal)[1];
    column2.vertical = std::vector<layout::ContainerDefinition>(3);
    layout::ContainerDefinition& row1 = (*column2.vertical)[0];
    row1.size = layout::Size { {}, layout::Dimension{ 168 } };
    layout::ContainerDefinition& row2 = (*column2.vertical)[1];
    layout::ContainerDefinition& row3 = (*column2.vertical)[2];
    row3.size = layout::Size { {}, layout::Dimension{ 100 } };

    layout::ContainerDefinition& column3 = (*root.horizontal)[2];
    column3.size = layout::Size { layout::Dimension{ 100 }, {} };

    services::LayoutService layoutService;
    constexpr objects::layout::Size availableSpace{ 1024, 768 };
    const objects::layout::Container container = layoutService.calculate(root, availableSpace);

    EXPECT_EQ(container.name, root.name);
    EXPECT_EQ(container.children.size(), 3);

    assertContainer(container.children[0], column1.name, 0, 0, 124, availableSpace.height);
    assertContainer(container.children[2], column3.name, 924, 0, 100, availableSpace.height);

    auto& column2Container = container.children[1];
    assertContainer(column2Container, column2.name, 124, 0, 800, availableSpace.height);
    EXPECT_EQ(column2Container.children.size(), 3);
    assertContainer(column2Container.children[0], row1.name, 0, 0, column2Container.size.width, 168);
    assertContainer(column2Container.children[1], row2.name, 0, 168, column2Container.size.width, 500);
    assertContainer(column2Container.children[2], row3.name, 0, 668, column2Container.size.width, 100);
}

TEST(LayoutTests, stretchContainerOversized)
{
    layout::ContainerDefinition root {};
    root.name = "root";
    root.horizontal = std::vector<layout::ContainerDefinition>{};

    root.size = layout::Size {
        {}, layout::Dimension{}
    };

    root.horizontal->reserve(1);

    layout::ContainerDefinition& column1 = root.horizontal->emplace_back();
    column1.size = layout::Size { {}, layout::Dimension{ 3000 } };

    services::LayoutService layoutService;
    constexpr objects::layout::Size availableSpace{ 1024, 768 };
    const objects::layout::Container container = layoutService.calculate(root, availableSpace);

    assertContainer(container, root.name, 0, 0, availableSpace.width, 3000);
    EXPECT_EQ(container.children.size(), 1);

    auto& innerContainer = container.children[0];
    assertContainer(innerContainer, column1.name, 0, 0, availableSpace.width, 3000);
}

TEST(LayoutTests, horizontalTiles)
{
    layout::ContainerDefinition root {};
    root.name = "root";
    root.horizontal = std::vector<layout::ContainerDefinition>(1);

    root.size = layout::Size {
        layout::Dimension{ 350 }, layout::Dimension{}
    };

    layout::ContainerDefinition& tilesColumn = (*root.horizontal)[0];
    tilesColumn.size = layout::Size { layout::Dimension{}, layout::Dimension{} };
    tilesColumn.horizontal = std::vector<layout::ContainerDefinition>(5);

    layout::ContainerDefinition& column1 = (*tilesColumn.horizontal)[0];
    column1.size = layout::Size { layout::Dimension{ 100 }, layout::Dimension{ 100 } };

    layout::ContainerDefinition& column2 = (*tilesColumn.horizontal)[1];
    column2.size = layout::Size { layout::Dimension{ 100 }, layout::Dimension{ 100 } };

    layout::ContainerDefinition& column3 = (*tilesColumn.horizontal)[2];;
    column3.size = layout::Size { layout::Dimension{ 100 }, layout::Dimension{ 100 } };

    layout::ContainerDefinition& column4 = (*tilesColumn.horizontal)[3];;
    column4.size = layout::Size { layout::Dimension{ 100 }, layout::Dimension{ 100 } };

    layout::ContainerDefinition& column5 = (*tilesColumn.horizontal)[4];;
    column5.size = layout::Size { layout::Dimension{ 100 }, layout::Dimension{ 100 } };

    services::LayoutService layoutService;
    constexpr objects::layout::Size availableSpace{ 1024, 768 };
    const objects::layout::Container container = layoutService.calculate(root, availableSpace);

    assertContainer(container, root.name, 337, 284, 350, 200);
    EXPECT_EQ(container.children.size(), 1);

    auto& tilesContainer = container.children[0];
    assertContainer(tilesContainer, tilesColumn.name, 0, 0, 300, 200);
    EXPECT_EQ(tilesContainer.children.size(), 5);

    auto& tile1 = tilesContainer.children[0];
    assertContainer(tile1, column1.name, 0, 0, 100, 100);

    auto& tile2 = tilesContainer.children[1];
    assertContainer(tile2, column2.name, 100, 0, 100, 100);

    auto& tile3 = tilesContainer.children[2];
    assertContainer(tile3, column3.name, 200, 0, 100, 100);

    auto& tile4 = tilesContainer.children[3];
    assertContainer(tile4, column4.name, 0, 100, 100, 100);

    auto& tile5 = tilesContainer.children[4];
    assertContainer(tile5, column5.name, 100, 100, 100, 100);
}

TEST(LayoutTests, verticalTiles)
{
    layout::ContainerDefinition root {};
    root.name = "root";
    root.horizontal = std::vector<layout::ContainerDefinition>(1);

    root.size = layout::Size {
        layout::Dimension{}, layout::Dimension{ 350 }
    };

    layout::ContainerDefinition& tilesColumn = (*root.horizontal)[0];;
    tilesColumn.size = layout::Size { layout::Dimension{}, layout::Dimension{} };
    tilesColumn.vertical = std::vector<layout::ContainerDefinition>(5);

    layout::ContainerDefinition& column1 = (*tilesColumn.vertical)[0];
    column1.size = layout::Size { layout::Dimension{ 100 }, layout::Dimension{ 100 } };

    layout::ContainerDefinition& column2 = (*tilesColumn.vertical)[1];
    column2.size = layout::Size { layout::Dimension{ 100 }, layout::Dimension{ 100 } };

    layout::ContainerDefinition& column3 = (*tilesColumn.vertical)[2];
    column3.size = layout::Size { layout::Dimension{ 100 }, layout::Dimension{ 100 } };

    layout::ContainerDefinition& column4 = (*tilesColumn.vertical)[3];
    column4.size = layout::Size { layout::Dimension{ 100 }, layout::Dimension{ 100 } };

    layout::ContainerDefinition& column5 = (*tilesColumn.vertical)[4];
    column5.size = layout::Size { layout::Dimension{ 100 }, layout::Dimension{ 100 } };

    services::LayoutService layoutService;
    constexpr objects::layout::Size availableSpace{ 1024, 768 };
    const objects::layout::Container container = layoutService.calculate(root, availableSpace);

    assertContainer(container, root.name, 412, 209, 200, 350);
    EXPECT_EQ(container.children.size(), 1);

    auto& tilesContainer = container.children[0];
    assertContainer(tilesContainer, tilesColumn.name, 0, 0, 200, 300);
    EXPECT_EQ(tilesContainer.children.size(), 5);

    auto& tile1 = tilesContainer.children[0];
    assertContainer(tile1, column1.name, 0, 0, 100, 100);

    auto& tile2 = tilesContainer.children[1];
    assertContainer(tile2, column2.name, 0, 100, 100, 100);

    auto& tile3 = tilesContainer.children[2];
    assertContainer(tile3, column3.name, 0, 200, 100, 100);

    auto& tile4 = tilesContainer.children[3];
    assertContainer(tile4, column4.name, 100, 0, 100, 100);

    auto& tile5 = tilesContainer.children[4];
    assertContainer(tile5, column5.name, 100, 100, 100, 100);
}

//TODO: write a test for a 3 column layout with a flexible column in the middle, which contains another three columns with a flexible column in the middle,
//where one of the columns has width defined as a percent of the parents width