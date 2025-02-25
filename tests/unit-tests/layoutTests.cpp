#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <dory/core/resources/scene/configuration.h>
#include <dory/core/resources/objects/layout.h>
#include <dory/core/services/layoutService.h>

using namespace dory;
using namespace dory::core;
using namespace dory::core::resources;
using namespace dory::core::resources::scene;
using namespace dory::core::resources::scene::configuration;

TEST(LayoutTests, layoutControl)
{
    layout::Container container1;

    //set size to available space in the parent
    container1.size = {};

    //set size by size of the content, both width and height
    container1.size = layout::Size{};

    container1.size = layout::Size {};
    //set width to available width in the parent
    container1.size->width = {};
    //set height by the height of the content
    container1.size->height = layout::Dimension{};
    container1.size->height->percents = {};
    container1.size->height->pixels = {};
    //set height to explicit value
    container1.size->height = layout::Dimension{};
    container1.size->height->percents = {};
    container1.size->height->pixels = { 100 };
}

TEST(LayoutTests, centeredPosition)
{
    layout::PositionedContainer root {};
    root.name = "root";

    root.size = layout::Size {
        layout::Dimension { 150 },
        layout::Dimension { 100 }
    };

    services::LayoutService layoutService;
    const objects::layout::Container container = layoutService.calculate(root, objects::layout::Size{ 350, 400 });

    EXPECT_EQ(container.name, root.name);
    EXPECT_EQ(container.size.width, 150);
    EXPECT_EQ(container.size.height, 100);
    EXPECT_EQ(container.position.x, 100);
    EXPECT_EQ(container.position.y, 150);
}

TEST(LayoutTests, originPosition)
{
    layout::PositionedContainer root {};
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

TEST(LayoutTests, explicitPosition)
{
    layout::PositionedContainer root {};
    root.name = "root";

    root.position = layout::Position {
        layout::Dimension { 12 },
        layout::Dimension { 225 }
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
    EXPECT_EQ(container.position.x, 12);
    EXPECT_EQ(container.position.y, 225);
}

TEST(LayoutTests, fullScreen)
{
    layout::PositionedContainer root {};
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
    layout::PositionedContainer root {};
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

void assertContainer(const objects::layout::Container& container, const Name& name, const std::size_t x, const std::size_t y,
    const std::size_t width, const std::size_t height)
{
    EXPECT_EQ(container.size.width, width);
    EXPECT_EQ(container.size.height, height);
    EXPECT_EQ(container.position.x, x);
    EXPECT_EQ(container.position.y, y);
}

TEST(LayoutTests, gridTwoColumnsLine)
{
    layout::PositionedContainer root {};
    root.name = "root";

    layout::FloatingContainer& column1 = root.horizontal.emplace_back();
    column1.size = layout::Size { layout::Dimension{ 124 }, {} }; //124px width, 100% height

    layout::FloatingContainer& column2 = root.horizontal.emplace_back(); //100% - 124px width, 100% height

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
    layout::PositionedContainer root {};
    root.name = "root";

    layout::FloatingContainer& row1 = root.vertical.emplace_back();
    row1.size = layout::Size { {}, layout::Dimension{ 168 } };

    layout::FloatingContainer& row2 = root.vertical.emplace_back();

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
    layout::PositionedContainer root {};
    root.name = "root";

    layout::FloatingContainer& column1 = root.horizontal.emplace_back();
    column1.size = layout::Size { layout::Dimension{ 124 }, {} };

    layout::FloatingContainer& column2 = root.horizontal.emplace_back();

    layout::FloatingContainer& column3 = root.horizontal.emplace_back();
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
    layout::PositionedContainer root {};
    root.name = "root";

    layout::FloatingContainer& row1 = root.vertical.emplace_back();
    row1.size = layout::Size { {}, layout::Dimension{ 168 } };

    layout::FloatingContainer& row2 = root.vertical.emplace_back();

    layout::FloatingContainer& row3 = root.vertical.emplace_back();
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
    layout::PositionedContainer root {};
    root.name = "root";

    layout::FloatingContainer& column1 = root.horizontal.emplace_back();
    column1.size = layout::Size { layout::Dimension{ 124 }, {} };

    layout::FloatingContainer& column2 = root.horizontal.emplace_back();
    layout::FloatingContainer& row1 = column2.vertical.emplace_back();
    row1.size = layout::Size { {}, layout::Dimension{ 168 } };
    layout::FloatingContainer& row2 = column2.vertical.emplace_back();
    layout::FloatingContainer& row3 = column2.vertical.emplace_back();
    row3.size = layout::Size { {}, layout::Dimension{ 100 } };

    layout::FloatingContainer& column3 = root.horizontal.emplace_back();
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
    layout::PositionedContainer root {};
    root.name = "root";

    root.size = layout::Size {
        {}, layout::Dimension{}
    };

    layout::FloatingContainer& column1 = root.horizontal.emplace_back();
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
    layout::PositionedContainer root {};
    root.name = "root";

    root.size = layout::Size {
        layout::Dimension{ 350 }, layout::Dimension{}
    };

    layout::FloatingContainer& tilesColumn = root.horizontal.emplace_back();
    tilesColumn.size = layout::Size { layout::Dimension{}, layout::Dimension{} };

    layout::FloatingContainer& column1 = tilesColumn.horizontal.emplace_back();
    column1.size = layout::Size { layout::Dimension{ 100 }, layout::Dimension{ 100 } };

    layout::FloatingContainer& column2 = tilesColumn.horizontal.emplace_back();
    column2.size = layout::Size { layout::Dimension{ 100 }, layout::Dimension{ 100 } };

    layout::FloatingContainer& column3 = tilesColumn.horizontal.emplace_back();
    column3.size = layout::Size { layout::Dimension{ 100 }, layout::Dimension{ 100 } };

    layout::FloatingContainer& column4 = tilesColumn.horizontal.emplace_back();
    column4.size = layout::Size { layout::Dimension{ 100 }, layout::Dimension{ 100 } };

    layout::FloatingContainer& column5 = tilesColumn.horizontal.emplace_back();
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
    layout::PositionedContainer root {};
    root.name = "root";

    root.size = layout::Size {
        layout::Dimension{}, layout::Dimension{ 350 }
    };

    layout::FloatingContainer& tilesColumn = root.horizontal.emplace_back();
    tilesColumn.size = layout::Size { layout::Dimension{}, layout::Dimension{} };

    layout::FloatingContainer& column1 = tilesColumn.vertical.emplace_back();
    column1.size = layout::Size { layout::Dimension{ 100 }, layout::Dimension{ 100 } };

    layout::FloatingContainer& column2 = tilesColumn.vertical.emplace_back();
    column2.size = layout::Size { layout::Dimension{ 100 }, layout::Dimension{ 100 } };

    layout::FloatingContainer& column3 = tilesColumn.vertical.emplace_back();
    column3.size = layout::Size { layout::Dimension{ 100 }, layout::Dimension{ 100 } };

    layout::FloatingContainer& column4 = tilesColumn.vertical.emplace_back();
    column4.size = layout::Size { layout::Dimension{ 100 }, layout::Dimension{ 100 } };

    layout::FloatingContainer& column5 = tilesColumn.vertical.emplace_back();
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